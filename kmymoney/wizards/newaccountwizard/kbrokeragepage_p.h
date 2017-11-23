/***************************************************************************
                             kbrokeragepage.cpp
                             -------------------
    begin                : Tue Sep 25 2006
    copyright            : (C) 2007 Thomas Baumgart
    email                : Thomas Baumgart <ipwizard@users.sourceforge.net>
                           (C) 2017 by Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KBROKERAGEPAGE_P_H
#define KBROKERAGEPAGE_P_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "ui_kbrokeragepage.h"

#include "wizardpage_p.h"

namespace NewAccountWizard
{
  class Wizard;

  class BrokeragePagePrivate : public WizardPagePrivate<Wizard>
  {
    Q_DISABLE_COPY(BrokeragePagePrivate)

  public:
    BrokeragePagePrivate(QObject* parent) :
      WizardPagePrivate<Wizard>(parent),
      ui(new Ui::KBrokeragePage)
    {
    }

    ~BrokeragePagePrivate()
    {
      delete ui;
    }

    Ui::KBrokeragePage *ui;
  };
}

#endif
