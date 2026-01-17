#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include <hidapi.h>

class Device : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Objects of this type are created and managed by the DeviceManager")

    Q_PROPERTY(unsigned short vendorId READ vendorId CONSTANT)
    Q_PROPERTY(unsigned short productId READ productId CONSTANT)
    Q_PROPERTY(QString serialNumber READ serialNumber CONSTANT)
    Q_PROPERTY(QString manufacturer READ manufacturer CONSTANT)
    Q_PROPERTY(QString product READ product CONSTANT)
    Q_PROPERTY(QString path READ path CONSTANT)

  public:
    explicit Device(hid_device_info *deviceInfo, QObject *parent = nullptr);

    unsigned short vendorId() const;
    unsigned short productId() const;
    const QString &serialNumber() const;
    const QString &manufacturer() const;
    const QString &product() const;
    const QString &path() const;

  private:
    unsigned short m_vendorId;
    unsigned short m_productId;
    QString m_serialNumber;
    QString m_manufacturer;
    QString m_product;
    QString m_path;
};

#endif // DEVICE_H