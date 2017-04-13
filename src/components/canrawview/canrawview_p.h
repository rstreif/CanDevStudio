#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include <memory>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHeaderView>

class CanRawViewPrivate : public QObject
{
    Q_OBJECT

public:
    void setupUi()
    {
        tvModel->setHorizontalHeaderLabels({tr("time"), tr("id"), tr("dlc"), tr("data")});
        tv->setModel(tvModel.get());
        tv->verticalHeader()->hideSection(0);

        toolbar->addWidget(pbClear.get());

        layout->addWidget(toolbar.get());
        layout->addWidget(tv.get());
    }

    std::unique_ptr<QVBoxLayout> layout { std::make_unique<QVBoxLayout>() };
    std::unique_ptr<QToolBar> toolbar { std::make_unique<QToolBar>() };
    std::unique_ptr<QTableView> tv { std::make_unique<QTableView>() };
    std::unique_ptr<QStandardItemModel> tvModel { std::make_unique<QStandardItemModel>(0, 4) };
    std::unique_ptr<QPushButton> pbClear { std::make_unique<QPushButton>("Clear") };
};

#endif // CANRAWVIEW_P_H
