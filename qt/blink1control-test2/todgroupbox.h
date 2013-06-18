#ifndef TODGROUPBOX_H
#define TODGROUPBOX_H

#include <QGroupBox>

namespace Ui {
class TodGroupBox;
}

class TodGroupBox : public QGroupBox
{
    Q_OBJECT
    
public:
    explicit TodGroupBox(QWidget *parent = 0);
    ~TodGroupBox();
    
private:
    Ui::TodGroupBox *ui;
};

#endif // TODGROUPBOX_H
