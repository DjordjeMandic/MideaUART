#include "Appliance/ApplianceBase.h"
#include "Frame/NetworkNotify.h"
#include "Helpers/Log.h"

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

namespace dudanov {
namespace midea {

static const char *TAG = "ApplianceBase";

ResponseStatus ApplianceBase::Request::callHandler(const Frame &frame) {
  if (!frame.hasType(this->requestType))
    return ResponseStatus::RESPONSE_WRONG;

  if (this->onData == nullptr)
    return RESPONSE_OK;

  return this->onData(frame.getData());
}

static bool read_frame(Frame &frame, Stream *stream) {
  while (stream->available()) {
    if (frame.deserialize(stream->read()))
      return true;
  }

  return false;
}

void ApplianceBase::setup() {
  m_timerManager.registerTimer(m_periodTimer);
  m_timerManager.registerTimer(m_networkTimer);
  m_timerManager.registerTimer(m_responseTimer);

  m_networkTimer.setCallback([this](Timer *timer) {
    m_sendNetworkNotify();
    timer->reset();
  });

  m_networkTimer.start(2 * 60 * 1000);
  m_networkTimer.call();
  m_setup();
}

void ApplianceBase::loop() {
  // Timers task
  m_timerManager.task();

  // Loop for appliances
  m_loop();

  // Frame receiving
  while (read_frame(m_receiver, m_stream)) {
    m_protocolID = m_receiver.getProtocol();

    LOG_D(TAG, "RX: %s", m_receiver.toString().c_str());

    m_handler(m_receiver);
    m_receiver.clear();
  }

  if (m_isBusy || m_isWaitForResponse())
    return;

  if (m_queue.empty()) {
    m_onIdle();
    return;
  }

  m_request = m_queue.front();
  m_queue.pop_front();

  LOG_D(TAG, "Getting and sending a request from the queue...");

  m_sendRequest(m_request);

  if (m_request->onData != nullptr) {
    m_resetAttempts();
    m_resetTimeout();

  } else {
    m_destroyRequest();
  }
}

void ApplianceBase::m_handler(const Frame &frame) {
  if (m_isWaitForResponse()) {
    auto result = m_request->callHandler(frame);

    if (result != RESPONSE_WRONG) {
      if (result == RESPONSE_OK) {
        if (m_request->onSuccess != nullptr)
          m_request->onSuccess();

        m_destroyRequest();

      } else {
        m_resetAttempts();
        m_resetTimeout();
      }

      return;
    }
  }

  // ignoring responses on network notifies
  if (frame.hasType(NETWORK_NOTIFY))
    return;

  /* HANDLE REQUESTS */
  if (frame.hasType(QUERY_NETWORK)) {
    m_sendNetworkNotify(QUERY_NETWORK);
    return;
  }

  m_onRequest(frame);
}

static uint8_t getSignalStrength() {
  const int32_t dbm = WiFi.RSSI();

  if (dbm > -63)
    return 4;

  if (dbm > -75)
    return 3;

  if (dbm > -88)
    return 2;

  return 1;
}

void ApplianceBase::m_sendNetworkNotify(FrameType msgType) {
  NetworkNotifyData notify{};
  notify.setConnected(WiFi.isConnected());
  notify.setSignalStrength(getSignalStrength());
  notify.setIP(WiFi.localIP());
  notify.appendCRC();

  if (msgType == NETWORK_NOTIFY) {
    LOG_D(TAG, "Enqueuing a DEVICE_NETWORK(0x0D) notification...");
    m_queueNotify(msgType, std::move(notify));

  } else {
    LOG_D(TAG, "Answer to QUERY_NETWORK(0x63) request...");
    m_sendFrame(msgType, std::move(notify));
  }
}

void ApplianceBase::m_resetTimeout() {
  m_responseTimer.setCallback([this](Timer *timer) {
    LOG_D(TAG, "Response timeout...");

    if (!--m_remainAttempts) {
      if (m_request->onError != nullptr)
        m_request->onError();

      m_destroyRequest();
      return;
    }

    LOG_D(TAG, "Sending request again. Attempts left: %d...", m_remainAttempts);

    m_sendRequest(m_request);
    m_resetTimeout();
  });

  m_responseTimer.start(m_timeout);
}

void ApplianceBase::m_destroyRequest() {
  LOG_D(TAG, "Destroying the request...");

  m_responseTimer.stop();
  delete m_request;
  m_request = nullptr;
}

void ApplianceBase::m_sendFrame(FrameType type, const FrameData &data) {
  auto frame = Frame::make(m_applianceID, m_protocolID, type, data);

  LOG_D(TAG, "TX: %s", frame.toString().c_str());

  m_stream->write(frame.data(), frame.size());
  m_isBusy = true;
  m_periodTimer.setCallback([this](Timer *timer) {
    m_isBusy = false;
    timer->stop();
  });

  m_periodTimer.start(m_period);
}

void ApplianceBase::m_queueRequest(FrameType type, FrameData &&data, ResponseHandler onData, Handler onSucess,
                                   Handler onError) {
  LOG_D(TAG, "Enqueuing the request...");

  m_queue.push_back(new Request{std::move(data), std::move(onData), std::move(onSucess), std::move(onError), type});
}

void ApplianceBase::m_queueRequestPriority(FrameType type, FrameData &&data, ResponseHandler onData, Handler onSucess,
                                           Handler onError) {
  LOG_D(TAG, "Priority request queuing...");

  m_queue.push_front(new Request{std::move(data), std::move(onData), std::move(onSucess), std::move(onError), type});
}

void ApplianceBase::setBeeper(bool value) {
  LOG_D(TAG, "Turning %s beeper feedback...", value ? "ON" : "OFF");

  m_beeper = value;
}

}  // namespace midea
}  // namespace dudanov
