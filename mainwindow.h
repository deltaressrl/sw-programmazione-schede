#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QSignalMapper>
#include "programsetting.h"
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include "qjsondocument.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "qprocess.h"

namespace Ui {
class MainWindow;
}

typedef enum
{
    STATO_IDLE=0,
    STATO_ERASE,
    STATO_PROGRAM,
    STATO_READ_DEVICES,
    STATO_EXPORT_HEX,
    STATO_VERIFY,
    STATO_RESET,
    STATO_QUERY,
    LAST_STATO
} STATI_OPERAZIONI;

typedef struct
{
    uint16_t IDdispositivo;
    QString nomeFile;

} STRUTTURA_DISPOSITIVO;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void caricaOggetti();
    void caricaFile(QString stringaTitolo, QString *stringaFile);
    void gestisciStatoErase();
    void gestisciStatoProgram();



    QCheckBox *chkAttuatori[32];
    QSignalMapper *mapAttuatori;
    programSetting *progSetting;
    STRUTTURA_PROGRAMMA Programma;
    STRUTTURA_DISPOSITIVO *strutturaDispositivo[64];
    uint8_t statoOperazioni;
    uint8_t sottoStatoOperazioni;
    QTimer *timOperazioni;
    uint32_t timeoutProcesso;
    uint8_t dispositivoAttivo;
    float incrementoBarra;
    float valoreBarra;
    QJsonObject oggettoInserito;
    QJsonObject oggettoRoot;
    QProcess *processoBatch;
    QProcessEnvironment envProcesso;
    QTimer *timAttesaProcesso;
    bool batchFinito;
    uint32_t numeroOggettiJSON;

    Ui::MainWindow *ui;
private slots:
    void chkAttuatoriStateChanged(int indice);
    bool dispositivoSel();
    void timOperazioniScaduto();
    void erroreProcesso(QProcess::ProcessError errore);
    void partitoProcesso();
    void printOutput();
    void printError();
    void timProcessoScaduto();





    void on_btnSelezionaHexMAB_clicked();
    void on_btnAnnullaSelezioneMAB_clicked();
    void on_chkMAB_stateChanged(int arg1);
    void on_chkAsseX_stateChanged(int arg1);
    void on_chkAsseY_stateChanged(int arg1);
    void on_btnSelezionaHexAssi_clicked();
    void on_btnAnnullaSelezioneAssi_clicked();
    void on_chkCover1_stateChanged(int arg1);
    void on_chkCover2_stateChanged(int arg1);
    void on_btnSelezionaHexCover_clicked();
    void on_btnAnnullaSelezioneCover_clicked();
    void on_btnSelezionaHexAttuatori_clicked();
    void on_btnAnnullaSelezioneAttuatori_clicked();
    void on_chkContainer1_stateChanged(int arg1);
    void on_chkContainer2_stateChanged(int arg1);
    void on_chkContainer3_stateChanged(int arg1);
    void on_chkContainer4_stateChanged(int arg1);
    void on_btnSelezionaHexContainer_clicked();
    void on_btnAnnullaSelezioneContainer_clicked();
    void on_chkAutocap_stateChanged(int arg1);
    void on_btnSelezionaHexAutocap_clicked();
    void on_btnAnnullaSelezioneAutocap_clicked();
    void on_chkCanLifter_stateChanged(int arg1);
    void on_btnSelezionaHexCanLifter_clicked();
    void on_btnAnnullaSelezioneCanLifter_clicked();
    void on_chkHumidifier_stateChanged(int arg1);
    void on_btnSelezionaHexHumidifier_clicked();
    void on_btnAnnullaSelezioneHumidifier_clicked();
    void on_btnSelezionaTuttoMAB_clicked();
    void on_btnSelezionaTuttoAssi_clicked();
    void on_btnSelezionaTuttoCover_clicked();
    void on_btnSelezionaTuttoContainer_clicked();
    void on_btnSelezionaTuttoAutocap_clicked();
    void on_btnSelezionaTuttoAttuatori_clicked();
    void on_btnSelezionaTuttoCanLifter_clicked();
    void on_btnSelezionaTuttoHumidifier_clicked();
    void on_btnErase_clicked();
    void on_btnQuery_clicked();
};

#endif // MAINWINDOW_H
