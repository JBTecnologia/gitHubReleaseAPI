/**
 ******************************************************************************
 * @file       createnewreleasedialog.h
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2014
 * @addtogroup gitHubAPI
 * @{
 * @addtogroup createNewReleaseDialog
 * @{
 * @brief Dialog that allows the user to input the info needed for creating a
 * new relese
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

#ifndef CREATENEWRELEASEDIALOG_H
#define CREATENEWRELEASEDIALOG_H

#include <QDialog>
#include <githubreleaseapi.h>

namespace Ui {
class createNewReleaseDialog;
}

class createNewReleaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit createNewReleaseDialog(QWidget *parent = 0);
    ~createNewReleaseDialog();
    gitHubReleaseAPI::newGitHubRelease getReleaseInfo();
private:
    Ui::createNewReleaseDialog *ui;
};

#endif // CREATENEWRELEASEDIALOG_H
