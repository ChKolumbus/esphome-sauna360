#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"

#ifdef USE_TIME
#include "esphome/components/time/real_time_clock.h"
#include "esphome/core/time.h"
#endif

#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif

#include <queue>
#include <map>
#include <functional>

namespace esphome {
namespace sauna360 {

class SAUNA360Listener {
 public:
   virtual void on_temperature(uint16_t temperature){};
   virtual void on_temperature_setting(uint16_t temperature_setting){};
   virtual void on_remaining_time(uint16_t remaining_time){};
};

class SAUNA360Component : public uart::UARTDevice, public Component {
  public:
    void setup() override;
    void loop() override;
    void dump_config() override;
    void register_listener(SAUNA360Listener *listener) { this->listeners_.push_back(listener); }
   #ifdef USE_BUTTON
    void set_elite_heater_on_button(button::Button *button) { this->set_elite_heater_on_button_ = button; };
    button::Button *set_elite_heater_on_button_{nullptr};
    void set_elite_heater_off_button(button::Button *button) { this->set_elite_heater_off_button_ = button; };
    button::Button *set_elite_heater_off_button_{nullptr};
    void set_elite_heater_standby_button(button::Button *button) { this->set_elite_heater_standby_button_ = button; };
    button::Button *set_elite_heater_standby_button_{nullptr};
    void set_pure_power_toggle_button(button::Button *button) { this->set_pure_power_toggle_button_ = button; };
    button::Button *set_pure_power_toggle_button_{nullptr};
   #endif
    void send();
    void apply_elite_heater_on_action();
    void apply_elite_heater_off_action();
    void apply_elite_heater_standby_action();
    void apply_pure_power_toggle_action();
   #ifdef USE_TIME
    void set_time_id(time::RealTimeClock *time_id) { this->time_id_ = time_id; }
    void set_time_device_address(uint16_t address) { this->time_device_address_ = address; }
    void send_time() { this->send_time_requested_ = true; }
   #endif

  protected:
    void handle_char_(uint8_t c);
    void handle_frame_(std::vector<uint8_t> frame);
    void handle_packet_(std::vector<uint8_t> packet);
    std::vector<uint8_t> rx_message_;
    std::queue<std::vector<uint8_t>> tx_queue_;
    uint32_t last_rx_;
    uint32_t last_tx_;
    std::vector<SAUNA360Listener *> listeners_{};
   #ifdef USE_TIME
    time::RealTimeClock *time_id_{nullptr};
    uint16_t time_device_address_;
    bool send_time_requested_;
    bool do_send_time_();
   #endif
};


}  // namespace sauna360
}  // namespace esphome