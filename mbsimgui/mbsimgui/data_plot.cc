/*
    MBSimGUI - A fronted for MBSim.
    Copyright (C) 2017 Martin Förg

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <config.h>
#include "data_plot.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

namespace MBSimGUI {

  DataPlot::DataPlot(const QVector<double> &x_, const QVector<QVector<double>> &y_, const QString &spinBoxLabel, const QString &title, const QString &xLabel, const QString &yLabel, QWidget *parent) : QWidget(parent), x(x_), y(y_) {
      auto *layout = new QGridLayout;
      setLayout(layout);

      QLabel *label = new QLabel(spinBoxLabel);
      layout->addWidget(label,0,0);
      num = new QSpinBox;
      num->setValue(1);
      num->setMinimum(1);
      num->setMaximum(y.size());
      layout->addWidget(num,0,1);
      layout->setColumnStretch(2,1);

      plot = new QwtPlot(title, this);
      plot->setAxisTitle(QwtPlot::xBottom,xLabel);
      plot->setAxisTitle(QwtPlot::yLeft,yLabel);
      plot->setMinimumSize(300,200);
      layout->addWidget(plot,1,0,1,3);

      curve = new QwtPlotCurve("Curve 1");
      curve->setSamples(x,y[0]);
      curve->attach(plot);
      connect(num,QOverload<int>::of(&QSpinBox::valueChanged),this,&DataPlot::changePlot);
      connect(num,QOverload<int>::of(&QSpinBox::valueChanged),this,&DataPlot::numChanged);
  }

  void DataPlot::setSymbol(const QwtSymbol::Style &style, int size) {
    QwtSymbol *symbol = new QwtSymbol(style);
    symbol->setSize(size);
    curve->setSymbol(symbol);
  }

  void DataPlot::setAxisScale(int axisId, double min, double max, double stepSize) {
    plot->setAxisScale(axisId,min,max,stepSize);
  }

  void DataPlot::replot() {
    plot->replot();
  }

  void DataPlot::changePlot(int i) {
    curve->setSamples(x,y[i-1]);
    plot->replot();
  }

  void DataPlot::changeNum(int i) {
    num->setValue(i);
  }

}
