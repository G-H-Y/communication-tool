#ifndef PROGRESSBARVIEW_H
#define PROGRESSBARVIEW_H

#include <QDialog>
namespace Ui {
class PROGRESSBARVIEW;
}

class PROGRESSBARVIEW : public QDialog
{
    Q_OBJECT
public:
    explicit PROGRESSBARVIEW(QWidget *parent = 0);
    ~PROGRESSBARVIEW();
    void SetProgress(int progress);
    void SetText(const QString& editText);
    Ui::PROGRESSBARVIEW *ui;
};

#endif // PROGRESSBARVIEW_H
