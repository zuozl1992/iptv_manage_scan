#pragma once

#include <QWidget>

namespace Ui { class DbSetupDialog; }

namespace Iptv::Ui {

class DbSetupDialog : public QWidget
{
    Q_OBJECT

public:
    explicit DbSetupDialog(QWidget *parent = nullptr);
    ~DbSetupDialog();

    QString configPath() const;
    QString dbPath() const;

signals:
    void setupCompleted();

private slots:
    void onSelectConfig(bool checked = false);
    void onCreateConfig(bool checked = false);
    void onSelectDb(bool checked = false);
    void onCreateDb(bool checked = false);
    void onAccept(bool checked = false);

private:
    ::Ui::DbSetupDialog *ui;
    QString m_configPath;
    QString m_dbPath;
};

} // namespace Iptv::Ui
