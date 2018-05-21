#include <QObject>

class ThingsDoer : public QObject
{
    Q_OBJECT
    public slots:
        void onHandshaken();
        void onclientIdReceived(quint32 clientId);
        void onloginCountChanged(quint32 loginCount);
        void ontripcodeReceived(const QByteArray &tripcode);
        void onerror(const QString &str);
        void onserverResponse(bool isResult, const QString &command);
        void onloginDetailsRequested();
        void onwaitingForStageEntry();
        void stageEntrySuccessful();
        void ondisconnected();
    private:
        quint32 clientId;
        quint32 loginCount;
};
