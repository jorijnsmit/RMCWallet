#ifndef WALLETMAIN_H
#define WALLETMAIN_H

#include <QMainWindow>
#include <QLabel>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMenu>

#include <ripple/protocol/PublicKey.h>
#include <ripple/protocol/SecretKey.h>

#include "encryption.h"
#include "errors.h"

namespace Ui {
class WalletMain;
}

#include "msgkind.h"

struct KeyData {
    ripple::SecretKey secretKey;
    ripple::PublicKey publicKey;
    std::vector<unsigned char> encryptedKey;
    ripple::AccountID accountID;
};

typedef std::vector<std::vector<QString> > TxVector;

class WalletMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit WalletMain(QWidget *parent = nullptr);
    ~WalletMain();

private Q_SLOTS:
    // Network events
    void onConnected();
    void doReconnect();
    void onConnectionError(QAbstractSocket::SocketError error);
    void onTextMessageReceived(QString message);

    // Transaction form actions
    void on_clearButton_clicked();
    void on_previewButton_clicked();
    void on_sendButton_clicked();

    // Menu actions
    void on_actionExit_triggered();
    void on_actionImport_key_triggered();
    void on_actionExport_key_triggered();
    void on_actionGenerateNew_triggered();
    void on_actionEncrypt_wallet_triggered();
    void on_actionCopy_account_address_triggered();
    void on_actionReconnect_triggered();
    void on_actionAbout_triggered();

    // Transaction history actions
    void txItemClicked(int nRow, int nCol);


    void on_actionSwitch_account_triggered();

private:
    // Client UI pointer
    Ui::WalletMain *ui;

    QLabel balanceLabel;
    QLabel ledgerLabel;
    QLabel networkStatusLabel;

    // WebSocket connection
    QWebSocket m_webSocket;
    int nConnectAttempt = 0;

    // RPC Request ID
    int nRequestID = 1;

    // Fee and sequence
    int64_t nFee = 10;
    int64_t nFeeRef = 10;
    int64_t nLedger = -1;
    int64_t nReserve = 2500;
    QString ledgerHash;
    int64_t ledgerCloseTime = 0;
    int ledgerTransactions = 0;

    // Wallet data
    int nDeriveIterations = 0;
    std::string mRSAPubKey;
    std::vector<unsigned char> mSalt;
    std::vector<unsigned char> mRSACryptedkey;

    int nCurrentAccount = 0;
    std::vector<KeyData> keyStore;
    QJsonArray accounts;
    std::vector<int64_t> balances;
    std::vector<int> sequences;
    std::vector<TxVector> transactions;

    // RPC request map
    std::map<int, MessageType> reqMap;

    // ================= //

    // Connect to random server
    void socketConnect();

    // Check internet availability
    bool isNetworkAvailable();

    // Update GUI to show new connection status
    void setOnline(bool flag = true, const QString& reason = "OK");


    // Response handlers

    void accInfoResponse(QJsonObject obj);
    void accTxResponse(QJsonObject obj);
    void submitResponse(QJsonObject obj);
    void subsLedgerAndAccountResponse(QJsonObject obj);

    // Request handlers
    void accInfoRequest(QJsonArray accs = {});
    void accTxRequest(QJsonArray accs = {});
    void submitRequest(QString hexBlobData);
    void subsLedgerAndAccountRequest();

    // Key management
    Error loadWallet();
    Error processWalletEntry(const QJsonObject& keyObj, KeyData& keyData);
    Error processWallet(const QJsonObject& keyObj);
    Error convertLegacyWallet(const QJsonObject& keyObj);
    void saveKeys(bool fOverwrite=false);
    Error newKey(QString& newAccountID);
    Error importKey(const secure::string& keyData, QString& newAccountID);
    Error exportKey(QString& strKey);

    // Ask for password
    Error askPassword();

    // Create and sign transaction
    Error createPaymentTx(const QString& receiverAccount, std::int64_t nAmount, std::int64_t nTransactionFee, std::int64_t nDestinationID, QString& dataJson, QString& dataHex);

    // Adjust element layout
    void setupControls(QWidget *parent);

    // Add transaction to table
    void processTxMessage(QJsonObject txObj);

    // Pull records from vector to data grid
    void refreshTxView();

    // Process ledger info
    void processLedgerMessage(QJsonObject ledgerObj);

    // Create and send payment
    void sendPayment(bool fJustAsk=true);
};

#endif // WALLETMAIN_H
