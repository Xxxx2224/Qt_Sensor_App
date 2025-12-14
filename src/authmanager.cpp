#include "authmanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <qcontainerfwd.h>
#include <qjsondocument.h>
#include <qlogging.h>

AuthManager::AuthManager(QObject *parent)
    : QObject(parent), m_isLoggedIn(false) {
  m_manager = new QNetworkAccessManager(this);


  loadSettings();
}

void AuthManager::loadSettings() {
  QSettings settings("BenimSirket", "YeniSensorApp");
  m_apiUrl = settings.value("api_url", "http://127.0.0.1:8000").toString();

  if (settings.contains("auth_token")) {
    m_token = settings.value("auth_token").toString();

    if (!m_token.isEmpty()) {
      qDebug() << "Token bulundu, sunucu kontrol ediliyor...";
      fetchSensorData();
    }
  }
}

void AuthManager::saveSettings() {
  QSettings settings("BenimSirket", "YeniSensorApp");
  settings.setValue("api_url", m_apiUrl);

}



QString AuthManager::apiUrl() const { return m_apiUrl; }

void AuthManager::setApiUrl(const QString &url) {
  if (m_apiUrl != url) {
    m_apiUrl = url;
    saveSettings();
    emit apiUrlChanged();
  }
}

bool AuthManager::isLoggedIn() const { return m_isLoggedIn; }
QString AuthManager::token() const { return m_token; }



void AuthManager::login(const QString &username, const QString &password) {

  QUrl url(m_apiUrl + "/token");

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");

  QUrlQuery postData;
  postData.addQueryItem("username", username);
  postData.addQueryItem("password", password);
  QByteArray data = postData.toString(QUrl::FullyEncoded).toUtf8();

  QNetworkReply *reply = m_manager->post(request, data);
  connect(reply, &QNetworkReply::finished,
          [this, reply]() { this->onLoginReply(reply); });
}

void AuthManager::onLoginReply(QNetworkReply *reply) {
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Giriş Hatası:" << reply->errorString();
    emit loginError("Hata: " + reply->errorString());
    reply->deleteLater();
    return;
  }

  QByteArray response = reply->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(response);
  QJsonObject obj = doc.object();

  if (obj.contains("access_token")) {
    m_token = obj["access_token"].toString();
    m_isLoggedIn = true;


    QSettings settings("BenimSirket", "YeniSensorApp");
    settings.setValue("auth_token", m_token);

    qDebug() << "Giriş Başarılı ve Kaydedildi!";
    emit tokenChanged();
    emit loginStatusChanged();
  } else {
    emit loginError("Kullanıcı adı/şifre hatalı");
  }
  reply->deleteLater();
}

void AuthManager::fetchSensorData() {

  if (m_token.isEmpty())
    return;

  QUrl url(m_apiUrl + "/sensor/latest");
  QNetworkRequest request(url);
  QString headerData = "Bearer " + m_token;
  request.setRawHeader("Authorization", headerData.toUtf8());

  QNetworkReply *reply = m_manager->get(request);


  connect(reply, &QNetworkReply::finished, [this, reply]() {

    if (reply->error() != QNetworkReply::NoError) {
      qDebug() << "Doğrulama Hatası:" << reply->errorString();


      m_isLoggedIn = false;



      emit loginError("Sunucuya ulaşılamadı veya oturum süresi doldu.");
      emit loginStatusChanged();
    }

    else {
      QByteArray response = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(response);
      QJsonObject obj = doc.object();


      if (obj.contains("temperature") && obj.contains("humidity")) {
        double temp = obj["temperature"].toDouble();
        double hum = obj["humidity"].toDouble();
        emit sensorDataReceived(temp, hum);


        if (!m_isLoggedIn) {
          m_isLoggedIn = true;
          emit loginStatusChanged();
          qDebug() << "Oturum doğrulandı, içeri alınıyor.";
        }
      }
    }
    reply->deleteLater();
  });
}

void AuthManager::onSensorReply(QNetworkReply *reply) {
  if (reply->error() != QNetworkReply::NoError) {

    reply->deleteLater();
    return;
  }

  QByteArray response = reply->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(response);
  QJsonObject obj = doc.object();

  if (obj.contains("temperature") && obj.contains("humidity")) {
    double temp = obj["temperature"].toDouble();
    double hum = obj["humidity"].toDouble();
    emit sensorDataReceived(temp, hum);
  }
  reply->deleteLater();
}

void AuthManager::logout() {
  m_isLoggedIn = false;
  m_token = "";


  QSettings settings("BenimSirket", "YeniSensorApp");
  settings.remove("auth_token");

  qDebug() << "Çıkış yapıldı ve kayıt silindi.";
  emit tokenChanged();
  emit loginStatusChanged();
}

void AuthManager::register_user(const QString &username,
                                const QString &password) {
  m_isLoggedIn = false;
  QSettings settings("BenimSirket", "YeniSensorApp");
  QUrl url(m_apiUrl + "/register");
  QNetworkRequest request(url);

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  QString headerData = "Bearer" + m_token;
  request.setRawHeader("Authorization", headerData.toUtf8());

  QJsonObject json;
  json["username"] = username;
  json["password"] = password;

  QJsonDocument doc(json);
  QByteArray data = doc.toJson();

  QNetworkReply *reply1 = m_manager->post(request, data);
  connect(reply1, &QNetworkReply::finished, [reply1]() {
    QByteArray response_data = reply1->readAll();
    QString ham_data = QString::fromUtf8(response_data);
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    if (reply1->error() == QNetworkReply::NoError) {
      qDebug() << "Yeni Kullanici başarıyla olusturuldu!";
    } else {
      qDebug() << "Yeni Kullanici Hatası:" << reply1->errorString();
      qDebug() << "Yeni Kullanici Hatasi" << QString::fromUtf8(response_data);
    }
    reply1->deleteLater();
  });
}
void AuthManager::sendCommand(const QString &target, bool state) {
  if (!m_isLoggedIn || m_token.isEmpty())
    return;

  QUrl url(m_apiUrl + "/control");
  QNetworkRequest request(url);

  // Header
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  QString headerData = "Bearer " + m_token;
  request.setRawHeader("Authorization", headerData.toUtf8());

  // JSON Body: {"device_id": "MAIN_BOARD", "target": "FAN", "command": "ON"}
  QJsonObject json;
  json["device_id"] = "MAIN_BOARD";
  json["target"] = target;
  json["command"] = state ? "ON" : "OFF";

  QJsonDocument doc(json);
  QByteArray data = doc.toJson();


  QNetworkReply *reply = m_manager->post(request, data);

  connect(reply, &QNetworkReply::finished, [reply]() {
    if (reply->error() == QNetworkReply::NoError) {
      qDebug() << "Komut başarıyla iletildi!";
    } else {
      qDebug() << "Komut Hatası:" << reply->errorString();
    }
    reply->deleteLater();
  });
}
