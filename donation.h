#ifndef DONATIONDIALOG_H
#define DONATIONDIALOG_H

#include "ui_donation.h"
#include "mainwindow.h"

class donationDialog : public QDialog, private Ui::Dialog_Donation
{
	Q_OBJECT

public:

	donationDialog(QWidget *parent);

private:

	double BTCexchangeRate;

private slots:

	double EUR2BTC();

	void on_horizontalSlider_valueChanged(int);
	void on_comboBox_activated(const QString &);
	void on_pushButton_clicked();

	void keyPressEvent(QKeyEvent*);
};

#endif
