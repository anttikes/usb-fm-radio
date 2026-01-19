#include "Device.h"

Device::Device(hid_device_info *deviceInfo)
{
    m_vendorId = deviceInfo->vendor_id;
    m_productId = deviceInfo->product_id;
    m_serialNumber = QString::fromWCharArray(deviceInfo->serial_number);
    m_manufacturer = QString::fromWCharArray(deviceInfo->manufacturer_string);
    m_product = QString::fromWCharArray(deviceInfo->product_string);
    m_path = QString::fromLatin1(deviceInfo->path);
}

unsigned short Device::vendorId() const
{
    return m_vendorId;
}

unsigned short Device::productId() const
{
    return m_productId;
}

const QString &Device::serialNumber() const
{
    return m_serialNumber;
}

const QString &Device::manufacturer() const
{
    return m_manufacturer;
}

const QString &Device::product() const
{
    return m_product;
}

const QString &Device::path() const
{
    return m_path;
}