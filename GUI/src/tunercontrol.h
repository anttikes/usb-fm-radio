#ifndef TUNERCONTROL_H
#define TUNERCONTROL_H

#include <QObject>

class CTunerControl : public QObject
{
    Q_OBJECT

  public:
    explicit CTunerControl(QObject *parent = nullptr);
    ~CTunerControl();

    bool initialize();
    void deInitialize();

  private:
    bool m_bInitialized;
};

#endif // TUNERCONTROL_H