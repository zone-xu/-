#ifndef INPUTIPEDIT_H
#define INPUTIPEDIT_H

#include <QLineEdit>

//**************************** IP地址输入框
class IPAddressEdit : public QLineEdit
{
    Q_OBJECT
public:
    IPAddressEdit(QWidget *parent = Q_NULLPTR);
    ~IPAddressEdit() override;

public:
    void setIPText(const QString &ipText);  //设置IP地址文本
    QString ipText() const; //获取IP地址文本
    void setPartReadOnly(bool isReadble);   //设置为只读
    bool isPartEmpty();   //是否为空

protected:
    void keyPressEvent(QKeyEvent *event) override;  //键盘输入重获焦点
    bool eventFilter(QObject *obj, QEvent *event) override; //小编辑框事件过滤器

private:
    void initIPAddrEdit();  //IP地址编辑框初始化
    void setSelection(QLineEdit *curEdit);    //选择小编辑框文本
    bool trunFocus(Qt::Key key, QLineEdit *curEdit);   //焦点跳转

private:
    QList<QLineEdit*> ipParts;  //小编辑框容器

    QLineEdit *nextEdit;    //要焦点跳转的小编辑框
    bool isPartReadOnly;    //是否只读
};

#endif // INPUTIPEDIT_H
