#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    caricaOggetti();
    progSetting=new programSetting(&Programma);
    statoOperazioni=STATO_IDLE;
    sottoStatoOperazioni=0;
    timOperazioni=new QTimer(this);
    connect(timOperazioni,SIGNAL(timeout()),this,SLOT(timOperazioniScaduto()),Qt::AutoConnection);
    timOperazioni->start(50);
    timeoutProcesso=0;
    processoBatch=new QProcess(this);
    envProcesso=QProcessEnvironment::systemEnvironment();
    connect(processoBatch,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(erroreProcesso(QProcess::ProcessError)),Qt::AutoConnection);
    connect(processoBatch,SIGNAL(started()),this,SLOT(partitoProcesso()),Qt::AutoConnection);
    connect (processoBatch, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
    connect (processoBatch, SIGNAL(readyReadStandardError()), this, SLOT(printError()));//    processoBatch->setWorkingDirectory(QCoreApplication::applicationDirPath());
    processoBatch->setProcessEnvironment(envProcesso);
    timAttesaProcesso=new QTimer(this);
    connect(timAttesaProcesso,SIGNAL(timeout()),this,SLOT(timProcessoScaduto()),Qt::AutoConnection);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::caricaOggetti()
{
    int i;
    int x,y;
    QString Stringa;

    x=20;
    y=40;

    // adesso impostiamo i check box per i coloranti
    mapAttuatori=new QSignalMapper(this);

    x=10;
    y=20;
    for (i=0;i<32;i++)
    {
        chkAttuatori[i]=new QCheckBox(ui->grpAttuatori);
        Stringa.sprintf("checkAttuatori%d",i+1);
        chkAttuatori[i]->setObjectName(Stringa);
        chkAttuatori[i]->setGeometry(QRect(x,y,71,17));
        Stringa.sprintf("Attuatore %d",i+1);
        chkAttuatori[i]->setText(Stringa);
        chkAttuatori[i]->setEnabled(true);
        chkAttuatori[i]->setVisible(true);
        connect(chkAttuatori[i],SIGNAL(stateChanged(int)),mapAttuatori,SLOT(map()),Qt::AutoConnection);
        mapAttuatori->setMapping(chkAttuatori[i],i);

        if ((((i+1)%8)==0) && (i>0))
        {
            x=10;
            y+=20;
        }
        else
        {
            x+=80;
        }
    }

    connect(mapAttuatori,SIGNAL(mapped(int)),this,SLOT(chkAttuatoriStateChanged(int)));

}

void MainWindow::chkAttuatoriStateChanged(int indice)
{
    // qui ci passo quando clicco sul check dei colori
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (chkAttuatori[indice]->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto attuatore %d\n",indice+1);
        Programma.programmaAttuatori[indice]=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato attuatore %d\n",indice+1);
        Programma.programmaAttuatori[indice]=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_btnSelezionaHexMAB_clicked()
{
    caricaFile("Apri programma MAB",&Programma.nomeFileMAB);

}

void MainWindow::caricaFile(QString stringaTitolo, QString *stringaFile)
{
    QFileDialog *fileDialog;
    QStringList nomeFile;
    QString Stringa;

    fileDialog=new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setNameFilter("File Hex (*.hex)");
    fileDialog->setViewMode(QFileDialog::Detail);
    fileDialog->setWindowTitle(stringaTitolo);
    if (fileDialog->exec())
    {
        // ha premuto ok - andiamo a vedere cosa ha scelto
        nomeFile=fileDialog->selectedFiles();
        Stringa=ui->txtStato->toPlainText();
        Stringa+="Caricato file "+nomeFile[0]+"\n";
        ui->txtStato->setText(Stringa);
        ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
        *stringaFile=nomeFile[0];
    }

}

void MainWindow::on_btnAnnullaSelezioneMAB_clicked()
{
    ui->chkMAB->setCheckState(Qt::Unchecked);
    Programma.programmaMAB=false;

    Programma.nomeFileMAB="";

}

void MainWindow::on_chkMAB_stateChanged(int arg1)
{
    // qui qualcuno ha cliccato sul check box della mab - dobbiamo vedere se seleziona o meno la MAB
//    arg1=1;
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkMAB->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunta MAB\n");
        Programma.programmaMAB=true;
    }
    else
    {
        Stringa1.sprintf("Eliminata MAB\n");
        Programma.programmaMAB=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_chkAsseX_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkAsseX->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunta asse X\n");
        Programma.programmaAssi[0]=true;
    }
    else
    {
        Stringa1.sprintf("Eliminata asse X\n");
        Programma.programmaAssi[0]=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_chkAsseY_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkAsseY->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunta asse Y\n");
        Programma.programmaAssi[1]=true;
    }
    else
    {
        Stringa1.sprintf("Eliminata asse Y\n");
        Programma.programmaAssi[1]=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_btnSelezionaHexAssi_clicked()
{
    caricaFile("Apri programma assi",&Programma.nomeFileAssi);
}

void MainWindow::on_btnAnnullaSelezioneAssi_clicked()
{
    int i;

    ui->chkAsseX->setCheckState(Qt::Unchecked);
    ui->chkAsseY->setCheckState(Qt::Unchecked);

    for (i=0;i<2;i++)
    {
        Programma.programmaAssi[i]=false;
    }

    Programma.nomeFileAssi="";

}

void MainWindow::on_chkCover1_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkCover1->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto Cover 1\n");
        Programma.programmaCover[0]=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato Cover 1\n");
        Programma.programmaCover[0]=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_chkCover2_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkCover2->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto Cover 2\n");
        Programma.programmaCover[1]=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato Cover 2\n");
        Programma.programmaCover[1]=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_btnSelezionaHexCover_clicked()
{
    caricaFile("Apri programma cover",&Programma.nomeFileCover);

}

void MainWindow::on_btnAnnullaSelezioneCover_clicked()
{
    int i;

    ui->chkCover1->setCheckState(Qt::Unchecked);
    ui->chkCover2->setCheckState(Qt::Unchecked);

    for (i=0;i<2;i++)
    {
        Programma.programmaCover[i]=false;
    }

    Programma.nomeFileCover="";

}

void MainWindow::on_btnSelezionaHexAttuatori_clicked()
{
    // devo selezionare il file corretto
    caricaFile("Apri programma attuatori",&Programma.nomeFileAttuatori);

}

void MainWindow::on_btnAnnullaSelezioneAttuatori_clicked()
{
    int i;

    for (i=0;i<32;i++)
    {
        chkAttuatori[i]->setCheckState(Qt::Unchecked);
        Programma.programmaAttuatori[i]=false;
    }

    Programma.nomeFileAttuatori="";

}

void MainWindow::on_chkContainer1_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkContainer1->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto Container 1\n");
        Programma.programmaContainer[0]=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato Container 1\n");
        Programma.programmaContainer[0]=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_chkContainer2_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkContainer2->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto Container 2\n");
        Programma.programmaContainer[1]=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato Container 2\n");
        Programma.programmaContainer[2]=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_chkContainer3_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkContainer3->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto Container 3\n");
        Programma.programmaContainer[2]=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato Container 3\n");
        Programma.programmaContainer[2]=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_chkContainer4_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkContainer4->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto Container 4\n");
        Programma.programmaContainer[3]=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato Container 4\n");
        Programma.programmaContainer[3]=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_btnSelezionaHexContainer_clicked()
{
    caricaFile("Apri programma container",&Programma.nomeFileContainer);

}

void MainWindow::on_btnAnnullaSelezioneContainer_clicked()
{
    int i;

    ui->chkContainer1->setCheckState(Qt::Unchecked);
    ui->chkContainer2->setCheckState(Qt::Unchecked);
    ui->chkContainer3->setCheckState(Qt::Unchecked);
    ui->chkContainer4->setCheckState(Qt::Unchecked);

    for (i=0;i<4;i++)
    {
        Programma.programmaContainer[i]=false;
    }

    Programma.nomeFileContainer="";

}

void MainWindow::on_chkAutocap_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkAutocap->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto Autocap\n");
        Programma.programmaAutocap=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato Autocap\n");
        Programma.programmaAutocap=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_btnSelezionaHexAutocap_clicked()
{
    caricaFile("Apri programma autocap",&Programma.nomeFileAutocap);

}

void MainWindow::on_btnAnnullaSelezioneAutocap_clicked()
{
    ui->chkAutocap->setCheckState(Qt::Unchecked);

    Programma.programmaAutocap=false;

    Programma.nomeFileAutocap="";

}

void MainWindow::on_chkCanLifter_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkCanLifter->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto Can lifter\n");
        Programma.programmaCanLifter=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato Can lifter\n");
        Programma.programmaCanLifter=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_btnSelezionaHexCanLifter_clicked()
{
    caricaFile("Apri programma can lifter",&Programma.nomeFileCanLifter);

}

void MainWindow::on_btnAnnullaSelezioneCanLifter_clicked()
{
    ui->chkCanLifter->setCheckState(Qt::Unchecked);

    Programma.programmaCanLifter=false;

    Programma.nomeFileCanLifter="";

}

void MainWindow::on_chkHumidifier_stateChanged(int arg1)
{
    QString Stringa;
    QString Stringa1;

    Stringa=ui->txtStato->toPlainText();
    if (ui->chkHumidifier->checkState()==Qt::Checked)
    {
        Stringa1.sprintf("Aggiunto humidifier\n");
        Programma.programmaHumidifier=true;
    }
    else
    {
        Stringa1.sprintf("Eliminato humidifier\n");
        Programma.programmaHumidifier=false;
    }
    Stringa+=Stringa1;
    ui->txtStato->setText(Stringa);
    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

}

void MainWindow::on_btnSelezionaHexHumidifier_clicked()
{
    caricaFile("Apri programma humidifier",&Programma.nomeFileHumidifier);
}

void MainWindow::on_btnAnnullaSelezioneHumidifier_clicked()
{
    ui->chkHumidifier->setCheckState(Qt::Unchecked);

    Programma.programmaHumidifier=false;

    Programma.nomeFileHumidifier="";

}


void MainWindow::on_btnSelezionaTuttoMAB_clicked()
{
    // seleziono la MAB
    ui->chkMAB->setCheckState(Qt::Checked);
}

void MainWindow::on_btnSelezionaTuttoAssi_clicked()
{
    ui->chkAsseX->setCheckState(Qt::Checked);
    ui->chkAsseY->setCheckState(Qt::Checked);
}

void MainWindow::on_btnSelezionaTuttoCover_clicked()
{
    ui->chkCover1->setCheckState(Qt::Checked);
    ui->chkCover2->setCheckState(Qt::Checked);
}

void MainWindow::on_btnSelezionaTuttoContainer_clicked()
{
    ui->chkContainer1->setCheckState(Qt::Checked);
    ui->chkContainer2->setCheckState(Qt::Checked);
    ui->chkContainer3->setCheckState(Qt::Checked);
    ui->chkContainer4->setCheckState(Qt::Checked);
}


void MainWindow::on_btnSelezionaTuttoAutocap_clicked()
{
    ui->chkAutocap->setCheckState(Qt::Checked);
}

void MainWindow::on_btnSelezionaTuttoAttuatori_clicked()
{
    for (int i=0;i<32;i++)
    {
        chkAttuatori[i]->setCheckState(Qt::Checked);
    }
}


void MainWindow::on_btnSelezionaTuttoCanLifter_clicked()
{
    ui->chkCanLifter->setCheckState(Qt::Checked);
}


void MainWindow::on_btnSelezionaTuttoHumidifier_clicked()
{
    ui->chkHumidifier->setCheckState(Qt::Checked);
}

void MainWindow::on_btnErase_clicked()
{
    // dobbiamo creare il file json per far partire il reset
    if (!dispositivoSel())
    {
        // non è stato selezionato nessun dispositivo - non faccio nulla
        QMessageBox msgBox;

        msgBox.setText("Non è stato selezionato nessun dispositivo!!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return;
    }

    // ci sono dei dispositivi selezionati procedo con il processo di erase
    ui->btnDumpMemory->setEnabled(false);
    ui->btnErase->setEnabled(false);
    ui->btnExportHex->setEnabled(false);
    ui->btnProgramVerify->setEnabled(false);
    ui->btnQuery->setEnabled(false);
    ui->btnReadDevices->setEnabled(false);
    ui->btnResetDevice->setEnabled(false);
    ui->btnVerify->setEnabled(false);

    statoOperazioni=STATO_ERASE;
    sottoStatoOperazioni=0;

}

bool MainWindow::dispositivoSel()
{
    bool dispositivoSelezionato=false;
    int i;
    int indice=0;

    for (i=0;i<64;i++)
    {
        strutturaDispositivo[i]=nullptr;
    }

    if (Programma.programmaMAB)
    {
        dispositivoSelezionato=true;
        strutturaDispositivo[indice]=new STRUTTURA_DISPOSITIVO;
        strutturaDispositivo[indice]->IDdispositivo=0xFF;
        strutturaDispositivo[indice]->nomeFile=Programma.nomeFileMAB;
        indice++;
    }

    for (i=0;i<2;i++)
    {
        if (Programma.programmaAssi[i])
        {
            dispositivoSelezionato=true;
            strutturaDispositivo[indice]=new STRUTTURA_DISPOSITIVO;
            strutturaDispositivo[indice]->IDdispositivo=0x21+i;
            strutturaDispositivo[indice]->nomeFile=Programma.nomeFileAssi;
            indice++;
        }
    }

    for (i=0;i<2;i++)
    {
        if (Programma.programmaCover[i])
        {
            strutturaDispositivo[indice]=new STRUTTURA_DISPOSITIVO;
            strutturaDispositivo[indice]->IDdispositivo=0x27+i;
            strutturaDispositivo[indice]->nomeFile=Programma.nomeFileCover;
            indice++;
            dispositivoSelezionato=true;
        }
    }

    for (i=0;i<32;i++)
    {
        if (Programma.programmaAttuatori[i])
        {
            strutturaDispositivo[indice]=new STRUTTURA_DISPOSITIVO;
            strutturaDispositivo[indice]->IDdispositivo=0x09+i;
            strutturaDispositivo[indice]->nomeFile=Programma.nomeFileAttuatori;
            indice++;
            dispositivoSelezionato=true;
        }
    }

    for (i=0;i<4;i++)
    {
        if (Programma.programmaContainer[i])
        {
            strutturaDispositivo[indice]=new STRUTTURA_DISPOSITIVO;
            strutturaDispositivo[indice]->IDdispositivo=0x23+i;
            strutturaDispositivo[indice]->nomeFile=Programma.nomeFileContainer;
            indice++;
            dispositivoSelezionato=true;
        }
    }

    if (Programma.programmaAutocap)
    {
        strutturaDispositivo[indice]=new STRUTTURA_DISPOSITIVO;
        strutturaDispositivo[indice]->IDdispositivo=0x29;
        strutturaDispositivo[indice]->nomeFile=Programma.nomeFileAutocap;
        indice++;
        dispositivoSelezionato=true;
    }

    if (Programma.programmaCanLifter)
    {
        strutturaDispositivo[indice]=new STRUTTURA_DISPOSITIVO;
        strutturaDispositivo[indice]->IDdispositivo=0x2A;
        strutturaDispositivo[indice]->nomeFile=Programma.nomeFileCanLifter;
        indice++;
        dispositivoSelezionato=true;
    }

    if (Programma.programmaHumidifier)
    {
        strutturaDispositivo[indice]=new STRUTTURA_DISPOSITIVO;
        strutturaDispositivo[indice]->IDdispositivo=0x2B;
        strutturaDispositivo[indice]->nomeFile=Programma.nomeFileHumidifier;
        indice++;
        dispositivoSelezionato=true;
    }

    return dispositivoSelezionato;
}

void MainWindow::timOperazioniScaduto()
{
    timeoutProcesso+=50;

    switch (statoOperazioni)
    {
    case STATO_IDLE:
        // in questo stato sono a riposo - metto a 0 la barra
        ui->progressBar->setValue(0);
        ui->btnDumpMemory->setEnabled(true);
        ui->btnErase->setEnabled(true);
        ui->btnExportHex->setEnabled(true);
        ui->btnProgramVerify->setEnabled(true);
        ui->btnQuery->setEnabled(true);
        ui->btnReadDevices->setEnabled(true);
        ui->btnResetDevice->setEnabled(true);
        ui->btnVerify->setEnabled(true);
        sottoStatoOperazioni=0;
        break;
    case STATO_ERASE:
        gestisciStatoErase();
        break;
    case STATO_PROGRAM:
        gestisciStatoProgram();
        break;
    case STATO_READ_DEVICES:
//        gestisciReadDevices();
        break;
    case STATO_EXPORT_HEX:
//        gestisciExportHex();
        break;
    case STATO_VERIFY:
//        gestisciVerify();
        break;
    case STATO_RESET:
//        gestisciReset();
        break;
    case STATO_QUERY:
//        gestisciQuery();
        break;
    default:
        statoOperazioni=STATO_IDLE;
        break;
    }

}

void MainWindow::gestisciStatoProgram()
{
    int i;
    int numDevices;
    QString Stringa;
    QString Stringa1;
    QString nomeFileJson;
    QString pathApp = QCoreApplication::applicationDirPath();
    QString NomeFile = "leggiFilebatch.bat";
    QStringList arguments;
    QFile *fileJson;
    QString stringaJson;
    QJsonDocument *documentoJson;

    // in questa funione devo gestire la sequenza di operazioni per lo stato di erase - devo creare il file json con i
    // diversi slave su cui effettuare l'erase
    switch (sottoStatoOperazioni)
    {
    case 0:
        // in questo sottostato devo inizializzare la barra di scorrimento per avere un'idea di quando deve scorrere
        numDevices=0;
        i=0;
        while (strutturaDispositivo[i]!=nullptr)
        {
            numDevices++;
            i++;
        }
        incrementoBarra=100.0/(float)numDevices;
        valoreBarra=0;
        ui->progressBar->setValue((int)valoreBarra);
        sottoStatoOperazioni=1;
        timeoutProcesso=0;
        dispositivoAttivo=0;
        break;
    case 1:
        // per adesso faccio finta che ci siano i dispositivi da cancellare
/*        Stringa=ui->txtStato->toPlainText();
        if (strutturaDispositivo[dispositivoAttivo]->IDdispositivo==0xFF)
        {
            // eraso la MAB
            Stringa1="Cancellazione MAB attivata\n";
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=1) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=8))
        {
            // eraso una base
            Stringa1.sprintf("Cancellazione base %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo);
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=9) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=0x20))
        {
            // è un colore
            Stringa1.sprintf("Cancellazione colore %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo-8);
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=0x21) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=0x22))
        {
            // eraso gli assi X e Y
            Stringa1.sprintf("Cancellazione asse %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo-0x20);
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=0x23) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=0x26))
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione container %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo-0x22);
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=0x27) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=0x28))
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione cover %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo-0x26);
        }
        else if (strutturaDispositivo[dispositivoAttivo]->IDdispositivo==0x29)
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione autocap attivata\n");
        }
        else if (strutturaDispositivo[dispositivoAttivo]->IDdispositivo==0x2A)
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione CAN lifter attivata\n");
        }
        else if (strutturaDispositivo[dispositivoAttivo]->IDdispositivo==0x2B)
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione humidifier attivata\n");
        }

        Stringa+=Stringa1;
        ui->txtStato->setText(Stringa);

        ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
        sottoStatoOperazioni=2;
*/
        // qui costruisco il file JSON e lancio il file batch
        // per prima cosa costruisco il file json - il processo python si aspetta di trovare il file json nella stessa
        // directory in cui si trova lui
        nomeFileJson=QApplication::applicationDirPath();
        nomeFileJson+="/fileOperazioni.json";
        i=0;
        while (strutturaDispositivo[i]!=nullptr)
        {
            // costruisco gli oggetti JSON
            oggettoInserito["operazione"]="program";
            oggettoInserito["indSeriale"]=strutturaDispositivo[i]->IDdispositivo;
            oggettoInserito["nomeFile"]=strutturaDispositivo[i]->nomeFile;
            Stringa="operazione"+ Stringa1.sprintf("%d",i+1);
            i++;
            oggettoRoot.insert(Stringa,oggettoInserito);
        }
        numeroOggettiJSON=i;

        // in oggetto root ho tutte le operazioni di erase da fare
        documentoJson= new QJsonDocument(oggettoRoot);

        stringaJson=documentoJson->toJson();
        fileJson=new QFile(nomeFileJson);
        if (!fileJson->open(QIODevice::WriteOnly))
        {
            // non sono riuscito ad aprire il file, cacca!!
            QMessageBox msgbox;
            msgbox.setText("Non sono riuscito ad aprire il file JSON!!!!");
            msgbox.setIcon(QMessageBox::Critical);
            msgbox.exec();
            statoOperazioni=STATO_IDLE;
            sottoStatoOperazioni=0;
            break;
        }

        fileJson->write(stringaJson.toLocal8Bit());
        fileJson->close();

        // ho creato il file JSON adesso devo lanciare il processo batch
        pathApp+="/"+NomeFile;
        arguments << "/c" << pathApp;
        pathApp="cmd.exe";
        processoBatch->setWorkingDirectory(QCoreApplication::applicationDirPath());
        processoBatch->start(pathApp,arguments,QProcess::ReadWrite);
        sottoStatoOperazioni=2;
        batchFinito=false;
        break;
    case 2:
        // qui aspetto che il processo batch mi dica di avere finito
        if (batchFinito)
        {
            // il batch è finito chiudo la procedura di erase
            Stringa=ui->txtStato->toPlainText();
            Stringa1.sprintf("Fase di erase terminata\n");
            Stringa+=Stringa1;
            ui->txtStato->setText(Stringa);

            ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
            statoOperazioni=STATO_IDLE;
            // devo cancellare tutti gli oggetti
            for (i=1;i<=numeroOggettiJSON;i++)
            {
                Stringa="operazione"+Stringa1.sprintf("%d",i);
                oggettoRoot.remove(Stringa);
            }
            break;
        }

        break;

    default:
        sottoStatoOperazioni=0;
        break;
    }

}

void MainWindow::gestisciStatoErase()
{
    int i;
    int numDevices;
    QString Stringa;
    QString Stringa1;
    QString nomeFileJson;
    QString pathApp = QCoreApplication::applicationDirPath();
    QString NomeFile = "leggiFilebatch.bat";
    QStringList arguments;
    QFile *fileJson;
    QString stringaJson;
    QJsonDocument *documentoJson;

    // in questa funione devo gestire la sequenza di operazioni per lo stato di erase - devo creare il file json con i
    // diversi slave su cui effettuare l'erase
    switch (sottoStatoOperazioni)
    {
    case 0:
        // in questo sottostato devo inizializzare la barra di scorrimento per avere un'idea di quando deve scorrere
        numDevices=0;
        i=0;
        while (strutturaDispositivo[i]!=nullptr)
        {
            numDevices++;
            i++;
        }
        incrementoBarra=100.0/(float)numDevices;
        valoreBarra=0;
        ui->progressBar->setValue((int)valoreBarra);
        sottoStatoOperazioni=1;
        timeoutProcesso=0;
        dispositivoAttivo=0;
        break;
    case 1:
        // per adesso faccio finta che ci siano i dispositivi da cancellare
/*        Stringa=ui->txtStato->toPlainText();
        if (strutturaDispositivo[dispositivoAttivo]->IDdispositivo==0xFF)
        {
            // eraso la MAB
            Stringa1="Cancellazione MAB attivata\n";
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=1) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=8))
        {
            // eraso una base
            Stringa1.sprintf("Cancellazione base %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo);
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=9) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=0x20))
        {
            // è un colore
            Stringa1.sprintf("Cancellazione colore %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo-8);
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=0x21) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=0x22))
        {
            // eraso gli assi X e Y
            Stringa1.sprintf("Cancellazione asse %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo-0x20);
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=0x23) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=0x26))
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione container %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo-0x22);
        }
        else if ((strutturaDispositivo[dispositivoAttivo]->IDdispositivo>=0x27) && (strutturaDispositivo[dispositivoAttivo]->IDdispositivo<=0x28))
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione cover %d attivata\n",strutturaDispositivo[dispositivoAttivo]->IDdispositivo-0x26);
        }
        else if (strutturaDispositivo[dispositivoAttivo]->IDdispositivo==0x29)
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione autocap attivata\n");
        }
        else if (strutturaDispositivo[dispositivoAttivo]->IDdispositivo==0x2A)
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione CAN lifter attivata\n");
        }
        else if (strutturaDispositivo[dispositivoAttivo]->IDdispositivo==0x2B)
        {
            // eraso i container
            Stringa1.sprintf("Cancellazione humidifier attivata\n");
        }

        Stringa+=Stringa1;
        ui->txtStato->setText(Stringa);

        ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
        sottoStatoOperazioni=2;
*/
        // qui costruisco il file JSON e lancio il file batch
        // per prima cosa costruisco il file json - il processo python si aspetta di trovare il file json nella stessa
        // directory in cui si trova lui
        nomeFileJson=QApplication::applicationDirPath();
        nomeFileJson+="/fileOperazioni.json";
        i=0;
        while (strutturaDispositivo[i]!=nullptr)
        {
            // costruisco gli oggetti JSON
            oggettoInserito["operazione"]="erase";
            oggettoInserito["indSeriale"]=strutturaDispositivo[i]->IDdispositivo;
            oggettoInserito["nomeFile"]="";
            Stringa="operazione"+ Stringa1.sprintf("%d",i+1);
            i++;
            oggettoRoot.insert(Stringa,oggettoInserito);
        }
        numeroOggettiJSON=i;

        // in oggetto root ho tutte le operazioni di erase da fare
        documentoJson= new QJsonDocument(oggettoRoot);

        stringaJson=documentoJson->toJson();
        fileJson=new QFile(nomeFileJson);
        if (!fileJson->open(QIODevice::WriteOnly))
        {
            // non sono riuscito ad aprire il file, cacca!!
            QMessageBox msgbox;
            msgbox.setText("Non sono riuscito ad aprire il file JSON!!!!");
            msgbox.setIcon(QMessageBox::Critical);
            msgbox.exec();
            statoOperazioni=STATO_IDLE;
            sottoStatoOperazioni=0;
            break;
        }

        fileJson->write(stringaJson.toLocal8Bit());
        fileJson->close();

        // ho creato il file JSON adesso devo lanciare il processo batch
        pathApp+="/"+NomeFile;
        arguments << "/c" << pathApp;
        pathApp="cmd.exe";
        processoBatch->setWorkingDirectory(QCoreApplication::applicationDirPath());
        processoBatch->start(pathApp,arguments,QProcess::ReadWrite);
        sottoStatoOperazioni=2;
        batchFinito=false;
        break;
    case 2:
        // qui aspetto che il processo batch mi dica di avere finito
        if (batchFinito)
        {
            // il batch è finito chiudo la procedura di erase
            Stringa=ui->txtStato->toPlainText();
            Stringa1.sprintf("Fase di erase terminata\n");
            Stringa+=Stringa1;
            ui->txtStato->setText(Stringa);

            ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
            statoOperazioni=STATO_IDLE;
            // devo cancellare tutti gli oggetti
            for (i=1;i<=numeroOggettiJSON;i++)
            {
                Stringa="operazione"+Stringa1.sprintf("%d",i);
                oggettoRoot.remove(Stringa);
            }
            break;
        }

        break;

    default:
        sottoStatoOperazioni=0;
        break;
    }
}

void MainWindow::erroreProcesso(QProcess::ProcessError errore)
{
    QString Stringa;
    Stringa.sprintf("%d",(int)errore);

    // a seconda di dove sono devo resettare il processo
    QMessageBox msgbox;
    msgbox.setText("Errore processo:"+Stringa);
    msgbox.setIcon(QMessageBox::Critical);
    msgbox.exec();

    statoOperazioni=STATO_IDLE;
    sottoStatoOperazioni=0;
}

void MainWindow::partitoProcesso()
{
    timAttesaProcesso->start(100);

}

void MainWindow::timProcessoScaduto()
{
    if (processoBatch->state()==QProcess::NotRunning)
    {
        QMessageBox msgbox;
        msgbox.setText("Il processo batch è terminato!");
        msgbox.setIcon(QMessageBox::Information);
        msgbox.exec();
        timAttesaProcesso->stop();
        batchFinito=true;
    }
}

void MainWindow::printOutput()
{
    QByteArray byteArray = processoBatch->readAllStandardOutput();
    QStringList strLines = QString(byteArray).split("\n");



    foreach (QString line, strLines)
    {
        ui->txtStato->append(line);
    }

    ui->txtStato->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

    // devo andare a leggere cosa mi dive il processo in modo da capire a che punto è


}

void MainWindow::printError()
{
//    ui->txtPrintError->append("Got to printError()");

    QByteArray byteArray = processoBatch->readAllStandardError();
    QStringList strLines = QString(byteArray).split("\n");
/*
    foreach (QString line, strLines){
        ui->txtPrintError->append(line);
    }
*/

}

void MainWindow::on_btnQuery_clicked()
{

}
