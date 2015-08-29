/**
 ******************************************************************************
 * @file       mainwindow.cpp
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2015
 * @addtogroup gitHubAPI
 * @{
 * @addtogroup MainWindow
 * @{
 * @brief Small app to interface with the gitHubReleaseAPI
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressBar>
#include <QSettings>
#include "createnewreleasedialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    hideProgress(false)
{
    ui->setupUi(this);
    QSettings settings(qApp->applicationDirPath() + QDir::separator() + "settings.ini", QSettings::IniFormat);
    ui->repoOwner->setText(settings.value("owner", "").toString());
    ui->repoName->setText(settings.value("repo", "").toString());
    ui->username->setText(settings.value("username", "").toString());
    ui->password->setText(settings.value("password", "").toString());
    ui->menuView->addAction(ui->dockReleaseAssetDetails->toggleViewAction());
    ui->menuView->addAction(ui->dockReleaseDetails->toggleViewAction());
    ui->menuView->addAction(ui->dockUserDetails->toggleViewAction());
    ui->menuView->addAction(ui->dockReleaseAssetsBrowser->toggleViewAction());
    ui->menuView->addAction(ui->dockSettings->toggleViewAction());
    ui->dockSettings->close();
    api = new gitHubReleaseAPI(this);
    reloadReleases();
    connect(ui->releaseBrowser, SIGNAL(currentCellChanged(int,int,int,int)), SLOT(onCurrentReleasedChanged(int, int, int, int)));
    connect(ui->assetBrowser, SIGNAL(currentCellChanged(int,int,int,int)), SLOT(onCurrentAssetChanged(int, int, int, int)));
    statusProgress = new QProgressBar(this);
    statusLabel = new QLabel(this);
    statusLabel->setText("Uploading progress");
    statusProgress->setMaximum(100);
    statusProgress->setMinimum(0);
    this->statusBar()->addWidget(statusLabel);
    this->statusBar()->addWidget(statusProgress);
    statusLabel->setVisible(false);
    statusProgress->setVisible(false);
    connect(api, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(setProgressBarDownload(qint64,qint64)));
    connect(api, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(setProgressBarUpload(qint64,qint64)));
    connect(api, SIGNAL(logError(QString)), this, SLOT(setLogInfo(QString)));
    connect(api, SIGNAL(logInfo(QString)), this, SLOT(setLogInfo(QString)));
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onPictureDownloadDone(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reloadReleases()
{
    api->setRepo(ui->repoOwner->text(), ui->repoName->text());
    api->setCredentials(ui->username->text(), ui->password->text());
    releaseList = api->getReleases();
    loadReleases(releaseList);
}

void MainWindow::onCurrentReleasedChanged(int row, int column, int prow, int pcolumn)
{
    Q_UNUSED(prow);
    Q_UNUSED(pcolumn);
    Q_UNUSED(column);
    if(row == -1)
        return;
    if(ui->releaseBrowser->rowCount() < 1)
        return;
    int ID = ui->releaseBrowser->item(row, 0)->data(Qt::UserRole).toInt();
    loadRelease(releaseList.value(ID));
    loadAssets(releaseList.value(ID).assets);
    loadUser(releaseList.value(ID).author);
}

void MainWindow::onCurrentAssetChanged(int row, int column, int prow, int pcolumn)
{
    Q_UNUSED(prow);
    Q_UNUSED(pcolumn);
    Q_UNUSED(column);
    if(row == -1)
        return;
    gitHubReleaseAPI::GitHubAsset asset = releaseList.value(getCurrentReleaseID()).assets.value(getCurrentAssetID());
    loadAsset(asset);
    loadUser(asset.uploader);
}

void MainWindow::loadUser(gitHubReleaseAPI::GitHubUser user)
{
    currentUserID = user.id;
    ui->userLogin->setText(user.login);
    ui->userType->setText(user.type);
    ui->userUrl->setText(QString("<a href=\"%0\">%0</a>").arg(user.html_url.toString()));
    ui->userAdmin->setChecked(user.site_admin);
    if(userPics.keys().contains(currentUserID)) {
        ui->userPicture->setPixmap(userPics.value(currentUserID));
    }
    else {
        fileDownload(user.avatar_url);
    }
}

void MainWindow::loadRelease(gitHubReleaseAPI::release release)
{
    ui->releaseAuthor->setText(release.author.login);
    ui->releaseBody->setText(release.body);
    ui->releaseCreatedAt->setText(release.created_at.toString());
    ui->releaseDraft->setChecked(release.draft);
    ui->releaseHtmlUrl->setText(release.html_url.toString());
    ui->releaseID->setText(QString::number(release.id));
    ui->releaseName->setText(release.name);
    ui->releasePreRelease->setChecked(release.prerelease);
    ui->releasePublishedAt->setText(release.published_at.toString());
    ui->releaseTagName->setText(release.tag_name);
    ui->releaseTargetCommitish->setText(release.target_commitish);
}

void MainWindow::loadAssets(QHash<int, gitHubReleaseAPI::GitHubAsset> assetList)
{
    ui->assetBrowser->setRowCount(assetList.values().length());
    ui->assetBrowser->setColumnCount(5);
    int row = 0;
    QStringList labels;
    labels << "Name"
           << "Label"
           << "Url"
           << "Created at"
           << "Uploader";

    ui->assetBrowser->setHorizontalHeaderLabels(labels);
    foreach (gitHubReleaseAPI::GitHubAsset asset, assetList.values()) {
        QTableWidgetItem *newItem = new QTableWidgetItem(asset.name);
        newItem->setData(Qt::UserRole, asset.id);
        ui->assetBrowser->setItem(row, 0, newItem);
        newItem = new QTableWidgetItem(asset.label);
        ui->assetBrowser->setItem(row, 1, newItem);
        newItem = new QTableWidgetItem(asset.browser_download_url.toString());
        ui->assetBrowser->setItem(row, 2, newItem);
        newItem = new QTableWidgetDateItem(asset.created_at.toString());
        ui->assetBrowser->setItem(row, 3, newItem);
        newItem = new QTableWidgetItem(asset.uploader.login);
        ui->assetBrowser->setItem(row, 4, newItem);
        ++row;
    }
    ui->assetBrowser->sortByColumn(3, Qt::AscendingOrder);
    ui->assetBrowser->setCurrentCell(0, 0);
    onCurrentAssetChanged(0, 0, 0, 0);
}

void MainWindow::loadAsset(gitHubReleaseAPI::GitHubAsset asset)
{
    ui->assetCreatedAt->setText(asset.created_at.toString());
    ui->assetDownloadCount->setText(QString::number(asset.download_count));
    ui->assetLabel->setText(asset.label);
    ui->assetName->setText(asset.name);
    ui->assetSize->setText(QString::number(asset.size));
    ui->assetState->setText(asset.state);
    ui->assetUpdatedAt->setText(asset.updated_at.toString());
    ui->assetUrl->setText(asset.browser_download_url.toString());
}

void MainWindow::loadReleases(QHash<int, gitHubReleaseAPI::release> releaseList)
{
    ui->assetBrowser->clear();
    ui->releaseBrowser->clear();
    ui->releaseBrowser->setRowCount(releaseList.values().length());
    ui->releaseBrowser->setColumnCount(6);
    int row = 0;
    QStringList labels;
    labels << "Name"
           << "Tag Name"
           << "Is Draft"
           << "Is PreRelease"
           << "Author"
           << "Creation Date";

    ui->releaseBrowser->setHorizontalHeaderLabels(labels);
    foreach (int i, releaseList.keys()) {
        QTableWidgetItem *newItem = new QTableWidgetItem(releaseList.value(i).name);
        newItem->setData(Qt::UserRole, releaseList.value(i).id);
        ui->releaseBrowser->setItem(row, 0, newItem);
        newItem = new QTableWidgetItem(releaseList.value(i).tag_name);
        ui->releaseBrowser->setItem(row, 1, newItem);
        newItem = new QTableWidgetItem();
        if(releaseList.value(i).draft)
            newItem->setCheckState(Qt::Checked);
        else
            newItem->setCheckState(Qt::Unchecked);
        ui->releaseBrowser->setItem(row, 2, newItem);
        newItem = new QTableWidgetItem();
        if(releaseList.value(i).prerelease)
            newItem->setCheckState(Qt::Checked);
        else
            newItem->setCheckState(Qt::Unchecked);
        ui->releaseBrowser->setItem(row, 3, newItem);
        newItem = new QTableWidgetItem(releaseList.value(i).author.login);
        ui->releaseBrowser->setItem(row, 4, newItem);
        newItem = new QTableWidgetDateItem(releaseList.value(i).created_at.toString());
        ui->releaseBrowser->setItem(row, 5, newItem);
        ++row;
    }
    if(ui->releaseBrowser->rowCount() > 0) {
        ui->releaseBrowser->sortByColumn(5, Qt::DescendingOrder);
        ui->releaseBrowser->setCurrentCell(0, 0);
        onCurrentReleasedChanged(0, 0, 0, 0);
    }
}

void MainWindow::fileDownload(QUrl url)
{
    QNetworkRequest request;
    request.setUrl(url);
    manager.get(request);
}

int MainWindow::getCurrentReleaseID()
{
    if(ui->releaseBrowser->rowCount() == 0)
        return -1;
    return ui->releaseBrowser->item(ui->releaseBrowser->currentRow(), 0)->data(Qt::UserRole).toInt();
}

int MainWindow::getCurrentAssetID()
{
    if(ui->assetBrowser->rowCount() == 0)
        return -1;
    return ui->assetBrowser->item(ui->assetBrowser->currentRow(), 0)->data(Qt::UserRole).toInt();
}

void MainWindow::processErrors(gitHubReleaseAPI::errors error)
{
    QString e;
    switch (error) {
    case gitHubReleaseAPI::NETWORK_ERROR:
        e = "Operation failed with network error";
        break;
    case gitHubReleaseAPI::TIMEOUT_ERROR:
        e = "Operation failed with a network timeout error";
        break;
    case gitHubReleaseAPI::PARSING_ERROR:
        e = "Operation failed with a JSON parsing error";
        break;
    case gitHubReleaseAPI::NO_ERROR:
        e = "Operation succeeded";
    default:
        e = "Operation failed with an unknown error";
        break;
    }
    this->statusBar()->showMessage(e, 5000);
}

void MainWindow::on_assetDetails_clicked()
{
    ui->dockReleaseAssetDetails->show();
}

void MainWindow::on_releaseDetailsTB_clicked()
{
    ui->dockReleaseDetails->show();
}

void MainWindow::on_releaseDelete_clicked()
{
  if(QMessageBox::question(this, "Are you sure?", "Are you sure you want to delete this release? This operation cannot be undone!") == QMessageBox::Yes) {
      int ID = getCurrentReleaseID();
      api->deleteRelease(ID);
  }
}

void MainWindow::on_assetDelete_clicked()
{
    if(QMessageBox::question(this, "Are you sure?", "Are you sure you want to delete this asset? This operation cannot be undone!") == QMessageBox::Yes) {
        int ID = getCurrentAssetID();
        api->deleteAsset(ID);
    }
}

void MainWindow::on_assetDownloadTB_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select directory where to save file"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty())
        return;
    QByteArray data = api->downloadAsset(getCurrentAssetID());
    if(data.length() > 0) {
        QFile file(dir + QDir::separator() + ui->assetName->text());
        if(!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, "Error", "Could not save file");
            return;
        }
        file.write(data);
        file.close();
    }
}

void MainWindow::on_assetCreate_clicked()
{
    QString assetLabel;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath());
    if(fileName.isEmpty())
        return;
    bool ok;
    QString text = QInputDialog::getText(this, tr("If you want to give a label for this asset fill the box below"),
                                         tr("Asset Label:"), QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty())
        assetLabel = text;
    api->uploadReleaseAsset(fileName, assetLabel, getCurrentReleaseID());
    reloadReleases();
}

void MainWindow::on_releaseNew_clicked()
{
    createNewReleaseDialog dialog;
    if(dialog.exec() == QDialog::Rejected)
        return;
    gitHubReleaseAPI::newGitHubRelease rel = dialog.getReleaseInfo();
    api->createRelease(rel);
    reloadReleases();
}

void MainWindow::on_assetDownload_clicked()
{
    on_assetDownloadTB_clicked();
}

void MainWindow::on_assetUpdate_clicked()
{
    api->editAsset(getCurrentAssetID(), ui->assetName->text(), ui->assetLabel->text());
    reloadReleases();
}

void MainWindow::on_updateRelease_clicked()
{
    gitHubReleaseAPI::newGitHubRelease release;
    release.body = ui->releaseBody->toPlainText();
    release.draft = ui->releaseDraft->isChecked();
    release.name = ui->releaseName->text();
    release.prerelease = ui->releasePreRelease->isChecked();
    release.tag_name = ui->releaseTagName->text();
    release.target_commitish = ui->releaseTargetCommitish->text();
    api->editRelease(getCurrentReleaseID(), release);
    reloadReleases();
}

void MainWindow::setProgressBarUpload(qint64 progress, qint64 total)
{
    if(total == 0)
        return;
    statusLabel->setText("Upload Progress");
    int p = (progress * 100) / total;
    statusProgress->setValue(p);
    statusLabel->setVisible(p != 100);
    statusProgress->setVisible(p != 100);
}

void MainWindow::setProgressBarDownload(qint64 progress, qint64 total)
{
    if(total == 0)
        return;
    statusLabel->setText("Download Progress");
    int p = (progress * 100) / total;
    statusProgress->setValue(p);
    statusLabel->setVisible((p != 100) && !hideProgress);
    statusProgress->setVisible((p != 100) && !hideProgress);
}

void MainWindow::setLogInfo(QString str)
{
    QTimer::singleShot(2000, this, SLOT(reenableProgress()));
    statusProgress->setVisible(false);
    statusLabel->setVisible(false);
    hideProgress = true;
    statusBar()->showMessage(str, 2000);
}

void MainWindow::on_toolButton_clicked()
{
    api->setRepo("Taulabs", "Taulabs");
    api->setCredentials("guilhermito.santiago@gmail.com", "pedromiguel1248");
    releaseList = api->getReleases();
    loadReleases(releaseList);
}

void MainWindow::reenableProgress()
{
    hideProgress = false;
}

void MainWindow::onPictureDownloadDone(QNetworkReply *reply)
{
    QPixmap pix;
    pix.loadFromData(reply->readAll());
    ui->userPicture->setPixmap(pix);
    userPics.insert(currentUserID, pix);
    reply->deleteLater();
}

QTableWidgetDateItem::QTableWidgetDateItem(QString str, int type) : QTableWidgetItem(str, type)
{}

bool QTableWidgetDateItem::operator<(const QTableWidgetItem &rhs) const
{
    QDateTime rt = QDateTime::fromString(rhs.text(), Qt::TextDate);
    QDateTime lt = QDateTime::fromString(this->text(), Qt::TextDate);
    return lt < rt;
}

void MainWindow::on_saveSettings_clicked()
{
   QSettings settings(qApp->applicationDirPath() + QDir::separator() + "settings.ini", QSettings::IniFormat);
   settings.setValue("owner", ui->repoOwner->text());
   settings.setValue("repo", ui->repoName->text());
   settings.setValue("username", ui->username->text());
   settings.setValue("password", ui->password->text());
}

void MainWindow::on_reloadReleases_clicked()
{
    reloadReleases();
}
