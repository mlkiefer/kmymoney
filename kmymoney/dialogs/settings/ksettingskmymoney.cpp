/*
 * This file is part of KMyMoney, A Personal Finance Manager by KDE
 * Copyright (C) 2016 Christian Dávid <christian-david@web.de>
 * (C) 2017 by Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ksettingskmymoney.h"

#include <QPushButton>

#include <KPluginSelector>
#include <KLocalizedString>

#include "ksettingsgeneral.h"
#include "ksettingsregister.h"
#include "ksettingsgpg.h"
#include "ksettingscolors.h"
#include "ksettingsfonts.h"
#include "ksettingsicons.h"
#include "ksettingsschedules.h"
#include "ksettingsonlinequotes.h"
#include "ksettingshome.h"
#include "ksettingsforecast.h"
#include "ksettingsreports.h"

#include "pluginloader.h"
#include "icons/icons.h"

using namespace Icons;

KSettingsKMyMoney::KSettingsKMyMoney(QWidget *parent, const QString &name, KCoreConfigSkeleton *config)
    : KConfigDialog(parent, name, config)
{
  // create the pages ...
  const auto generalPage = new KSettingsGeneral();
  const auto registerPage = new KSettingsRegister();
  const auto homePage = new KSettingsHome();
  const auto schedulesPage = new KSettingsSchedules();
  const auto encryptionPage = new KSettingsGpg();
  const auto colorsPage = new KSettingsColors();
  const auto fontsPage = new KSettingsFonts();
  const auto iconsPage = new KSettingsIcons();
  const auto onlineQuotesPage = new KSettingsOnlineQuotes();
  const auto forecastPage = new KSettingsForecast();
  const auto pluginsPage = KMyMoneyPlugin::PluginLoader::instance()->pluginSelectorWidget();
  const auto reportsPage = new KSettingsReports();

  addPage(generalPage, i18nc("General settings", "General"), g_Icons[Icon::SystemRun]);
  addPage(homePage, i18n("Home"), g_Icons[Icon::ViewHome]);
  addPage(registerPage, i18nc("Ledger view settings", "Ledger"), g_Icons[Icon::ViewFinancialList]);
  addPage(schedulesPage, i18n("Scheduled transactions"), g_Icons[Icon::ViewSchedules]);
  addPage(onlineQuotesPage, i18n("Online Quotes"), g_Icons[Icon::PreferencesNetwork]);
  addPage(reportsPage, i18nc("Report settings", "Reports"), g_Icons[Icon::ViewReports]);
  addPage(forecastPage, i18nc("Forecast settings", "Forecast"), g_Icons[Icon::ViewForecast]);
  addPage(encryptionPage, i18n("Encryption"), g_Icons[Icon::Kgpg]);
  addPage(colorsPage, i18n("Colors"), g_Icons[Icon::PreferencesColor]);
  addPage(fontsPage, i18n("Fonts"), g_Icons[Icon::PreferencesFont]);
  addPage(iconsPage, i18n("Icons"), g_Icons[Icon::PreferencesIcon]);
  addPage(pluginsPage, i18n("Plugins"), g_Icons[Icon::NetworkDisconect]);

  setHelp("details.settings", "kmymoney");

  auto defaultButton = button(QDialogButtonBox::RestoreDefaults);
  connect(this, &KConfigDialog::rejected, schedulesPage, &KSettingsSchedules::slotResetRegion);
  connect(this, &KConfigDialog::rejected, iconsPage, &KSettingsIcons::slotResetTheme);
  connect(this, &KConfigDialog::settingsChanged, generalPage, &KSettingsGeneral::slotUpdateEquitiesVisibility);

  connect(this, &KConfigDialog::accepted, pluginsPage, &KPluginSelector::save);
  connect(defaultButton, &QPushButton::clicked, pluginsPage, &KPluginSelector::defaults);
}