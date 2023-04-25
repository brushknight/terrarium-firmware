#include "i2c.h"

namespace I2C
{
    i2c_port_t i2c_master_port = i2c_port_t(0);
    i2c_port_t i2c_slave_port = i2c_port_t(0);



const gpio_num_t I2C_MASTER_SCL_IO = gpio_num_t(22);        /*!< gpio number for I2C master clock */
const gpio_num_t I2C_MASTER_SDA_IO = gpio_num_t(21);     /*!< gpio number for I2C master data  */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define SLAVE_ADDRESS 0x0A

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */

const gpio_num_t I2C_SLAVE_SCL_IO = gpio_num_t(22);      /*!< gpio number for I2C master clock */
const gpio_num_t I2C_SLAVE_SDA_IO = gpio_num_t(21);      /*!< gpio number for I2C master data  */
#define I2C_SLAVE_FREQ_HZ 100000 /*!< I2C master clock frequency */
#define ESP_SLAVE_ADDR 0x0A

    esp_err_t i2c_master_init(void)
    {

        i2c_config_t conf;
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = I2C_MASTER_SDA_IO;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = I2C_MASTER_SCL_IO;
        conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

        esp_err_t err = i2c_param_config(i2c_master_port, &conf);
        if (err != ESP_OK)
        {
            return err;
        }
        return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    }

    esp_err_t i2c_slave_init(void)
    {

        i2c_config_t conf_slave;
        conf_slave.mode = I2C_MODE_SLAVE;
        conf_slave.sda_io_num = I2C_SLAVE_SDA_IO;
        conf_slave.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf_slave.scl_io_num = I2C_SLAVE_SCL_IO;
        conf_slave.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf_slave.slave.addr_10bit_en = 0;
        conf_slave.slave.slave_addr = ESP_SLAVE_ADDR;

        

        esp_err_t err = i2c_param_config(i2c_slave_port, &conf_slave);
        if (err != ESP_OK)
        {
            return err;
        }
        return i2c_driver_install(i2c_slave_port, conf_slave.mode, Message::jsonSize(), Message::jsonSize(), 0);
    }

    esp_err_t i2c_master_send(uint16_t i2cID, uint8_t message[], int len)
    {
        ESP_LOGD(TAG, "Sending Message = %s", message);

        esp_err_t ret;
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, i2cID << 1 | WRITE_BIT, ACK_CHECK_EN);
        i2c_master_write(cmd, message, len, ACK_CHECK_EN);
        i2c_master_stop(cmd);

        ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        return ret;
    }

    void write(uint16_t i2cID, Message message)
    {
        std::string messageJSON;
        serializeJson(message.toJSON(), messageJSON);
        i2c_master_send(i2cID, (uint8_t *)messageJSON.c_str(), message.jsonSize());
    }

    Message read()
    {

        Message message;

        uint8_t received_data[Message::jsonSize()] = {0};
        int bytesRead = i2c_slave_read_buffer(I2C::i2c_slave_port, received_data, Message::jsonSize(), 100 / portTICK_PERIOD_MS);
        if (bytesRead != Message::jsonSize())
        {
            i2c_reset_rx_fifo(I2C::i2c_slave_port);
            ESP_LOGW(TAG, "Raw data recived %d bytes, instead of %d, %s", bytesRead, Message::jsonSize(), received_data);
        }
        else
        {
            char *received_data2 = (char *)received_data;
            ESP_LOGD(TAG, "Raw data recived %d bytes, %s", bytesRead, received_data);

            // if (received_data2[0] != "{")
            // {
            //     ESP_LOGW(TAG, "Not a json payload, %s", received_data);
            // }

            message = Message::fromJSON(std::string(received_data2));
            message.bytes = bytesRead;
        }

        return message;
    }

}