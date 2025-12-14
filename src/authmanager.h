#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSettings>

class AuthManager : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString apiUrl READ apiUrl WRITE setApiUrl NOTIFY apiUrlChanged)
  Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY loginStatusChanged)
  Q_PROPERTY(QString token READ token NOTIFY tokenChanged)

public:
  explicit AuthManager(QObject *parent = nullptr);




  Q_INVOKABLE void login(const QString &username, const QString &password);


  Q_INVOKABLE void fetchSensorData();


  Q_INVOKABLE void logout();
  Q_INVOKABLE void register_user(const QString &username,
                                 const QString &password);

  Q_INVOKABLE void sendCommand(const QString &target, bool state);

  bool isLoggedIn() const;
  QString token() const;
  QString apiUrl() const;
  void setApiUrl(const QString &url);

signals:
  void sensorDataReceived(double temp, double humidity);
  void loginStatusChanged();
  void tokenChanged();
  void loginError(QString message);
  void apiUrlChanged();

private slots:
  void onLoginReply(QNetworkReply *reply);
  void onSensorReply(QNetworkReply *reply);

private:
  QNetworkAccessManager *m_manager;
  bool m_isLoggedIn;
  QString m_token;
  QString m_apiUrl;
  void loadSettings();
  void saveSettings();
};

#endif // AUTHMANAGER_H
