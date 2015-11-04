/**
 ******************************************************************************
 * @file       mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "githubreleaseapi.h"
#include <QProgressBar>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class QTableWidgetDateItem : public QObject, public QTableWidgetItem
{
    Q_OBJECT
public:
    QTableWidgetDateItem(QString str, int type = Type);
    bool operator< (const QTableWidgetItem& rhs) const;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void reloadReleases();
    void onCurrentReleasedChanged(int, int, int, int);
    void onCurrentAssetChanged(int, int, int, int);
    void on_assetDetails_clicked();
    void on_releaseDetailsTB_clicked();
    void on_releaseDelete_clicked();
    void on_assetDelete_clicked();
    void on_assetDownloadTB_clicked();
    void on_assetCreate_clicked();
    void on_releaseNew_clicked();
    void on_assetDownload_clicked();
    void on_assetUpdate_clicked();
    void on_updateRelease_clicked();
    void setProgressBarUpload(qint64 progress, qint64 total);
    void setProgressBarDownload(qint64 progress, qint64 total);
    void setLogInfo(QString str);
    void reenableProgress();
    void onPictureDownloadDone(QNetworkReply *reply);
    void on_saveSettings_clicked();

    void on_reloadReleases_clicked();

private:
    Ui::MainWindow *ui;
    gitHubReleaseAPI *api;
    void loadUser(gitHubReleaseAPI::GitHubUser user);
    void loadRelease(gitHubReleaseAPI::release release);
    void loadAssets(QHash<int, gitHubReleaseAPI::GitHubAsset> assetList);
    void loadAsset(gitHubReleaseAPI::GitHubAsset asset);
    void loadReleases(QHash<int, gitHubReleaseAPI::release> releaseList);
    QHash<int, gitHubReleaseAPI::release> releaseList;
    void fileDownload(QUrl url);
    int getCurrentReleaseID();
    int getCurrentAssetID();
    QProgressBar *statusProgress;
    QLabel *statusLabel;
    void processErrors(gitHubReleaseAPI::errors error);
    bool hideProgress;
    int currentUserID;
    QHash<int, QPixmap> userPics;
    QNetworkAccessManager manager;
};

#endif // MAINWINDOW_H
