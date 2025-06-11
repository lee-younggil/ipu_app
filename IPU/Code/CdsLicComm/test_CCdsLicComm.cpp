#include <QtTest>
#include <QSignalSpy>
#include "CdsLicComm.h"
#include "Code/Mediator.h"

class TestCCdsLicComm : public QObject
{
    Q_OBJECT

private:
    CMediator* m_mediator;
    CCdsLicComm* m_cdsLicComm;

private slots:
    // 테스트 초기화
    void initTestCase()
    {
        m_mediator = CMediator::getInstance();
        //m_mediator->initialization();
        m_cdsLicComm = CMediator::getInstance()->getCdsLicComm();
    }

    // 테스트 정리
    void cleanupTestCase()
    {
        //delete m_cdsLicComm;
        //delete m_mediator;
    }

    // 초기화 테스트
    void testInitialization()
    {
        m_cdsLicComm->initialization();
        QCOMPARE(m_cdsLicComm->getLastError(), CCdsLicComm::Status::OK);
        QVERIFY(m_cdsLicComm->getErrorMessage().isEmpty());
    }

    // 서비스 시작 테스트 - 정상 케이스
    void testServiceStartSuccess()
    {
        // 시리얼 포트가 정상적으로 열리는 상황 시뮬레이션
        bool result = m_cdsLicComm->ServiceStart();
        m_cdsLicComm->sendSerialData("Unit Test Start");
        QVERIFY(result);
        QCOMPARE(m_cdsLicComm->getLastError(), CCdsLicComm::Status::OK);
    }

    // 서비스 시작 테스트 - 실패 케이스
    void testServiceStartFailure()
    {
        // 시리얼 포트가 이미 사용 중인 상황 시뮬레이션
        // TODO: Mock 시리얼 포트를 사용하여 실패 상황 시뮬레이션
        bool result = m_cdsLicComm->ServiceStart();
        QVERIFY(!result);
        QCOMPARE(m_cdsLicComm->getLastError(), CCdsLicComm::Status::ERROR_PORT_UNAVAILABLE);
    }

    // 연결 상태 변경 시그널 테스트
    void testConnectionStatusSignal()
    {
        QSignalSpy spy(m_cdsLicComm, SIGNAL(serialConnectionStatusChanged(bool, QString)));
        
        // 연결 해제 상황 시뮬레이션
        m_cdsLicComm->ServiceStop();

        QTest::qWait(100);
        
        QCOMPARE(spy.count(), 1);
        QList<QVariant> arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).toBool(), false);

        m_cdsLicComm->ServiceStart();
        QTest::qWait(100);
    }

    // 로깅 테스트
    void testLogging()
    {
        QString testMessage = "Test log message";
        m_cdsLicComm->Log(testMessage);
        
        // 로그 파일이나 로그 시스템에서 메시지 확인
        // TODO: 로그 검증 로직 구현
    }

    // 에러 처리 테스트
    void testErrorHandling()
    {
        QString errorMsg = "Test error";
        m_cdsLicComm->setError(CCdsLicComm::Status::ERROR_COMMUNICATION, errorMsg);
        
        QCOMPARE(m_cdsLicComm->getLastError(), CCdsLicComm::Status::ERROR_COMMUNICATION);
        QCOMPARE(m_cdsLicComm->getErrorMessage(), errorMsg);
    }

    // 시리얼 통신 테스트
    void testSerialCommunication()
    {
        // 데이터 송신 테스트
        QString testData = "Unit Test Data 123";
        bool sendResult = m_cdsLicComm->sendSerialData(testData);
        QVERIFY(sendResult);

        // 데이터 수신 테스트
        QSignalSpy spy(m_cdsLicComm, SIGNAL(serialDataReceived(QString,QString)));
        
        // 수신 대기 (최대 5초)
        bool received = spy.wait(5000);
        
        qDebug() << "testSerialCommunication() - Data received:" << received;
        qDebug() << "testSerialCommunication() - Received count:" << spy.count();
        
        // 데이터가 수신되었는지 확인
        QVERIFY(received);
        QVERIFY(spy.count() > 0);
    }

    // 재연결 테스트
    void testReconnection()
    {
        // 연결 해제 후 재연결 시도
        m_cdsLicComm->ServiceStop();
        QTest::qWait(1000); // 잠시 대기
        
        bool reconnectResult = m_cdsLicComm->ServiceStart();
        QVERIFY(reconnectResult);
        qDebug() << "getLastError:" << int(m_cdsLicComm->getLastError());
        QCOMPARE(m_cdsLicComm->getLastError(), CCdsLicComm::Status::OK);
    }

    // 비정상 종료 복구 테스트
    void testRecoveryFromAbnormalTermination()
    {
        // 비정상 종료 상황 시뮬레이션
        m_cdsLicComm->ServiceStop();
        
        // 복구 시도
        bool recoveryResult = m_cdsLicComm->ServiceStart();
        QVERIFY(recoveryResult);
        QCOMPARE(m_cdsLicComm->getLastError(), CCdsLicComm::Status::OK);
    }
};

//QTEST_MAIN(TestCCdsLicComm)
//#include "test_CCdsLicComm.moc"
