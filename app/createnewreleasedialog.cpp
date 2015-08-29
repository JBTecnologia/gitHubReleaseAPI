/**
 ******************************************************************************
 * @file       createnewreleasedialog.cpp
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2015
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

#include "createnewreleasedialog.h"
#include "ui_createnewreleasedialog.h"

createNewReleaseDialog::createNewReleaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createNewReleaseDialog)
{
    ui->setupUi(this);
    this->setModal(true);
}

createNewReleaseDialog::~createNewReleaseDialog()
{
    delete ui;
}

gitHubReleaseAPI::newGitHubRelease createNewReleaseDialog::getReleaseInfo()
{
    gitHubReleaseAPI::newGitHubRelease rel;
    rel.body = ui->description->toPlainText();
    rel.draft = ui->isDraft->isChecked();
    rel.prerelease = ui->isPrerelease->isChecked();
    rel.name = ui->name->text();
    rel.tag_name = ui->tagName->text();
    rel.target_commitish = ui->commitish->text();
    return rel;
}
