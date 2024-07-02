#include "ipaddressedit.h"

#include <QValidator>
#include <QLabel>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QTime>
#include <QApplication>
#include <QDebug>

IPAddressEdit::IPAddressEdit(QWidget *parent)
    : QLineEdit (parent)
    , nextEdit (nullptr)
    , isPartReadOnly(false)
{
   for (int i = 0; i < 4; i++)
   {
       ipParts.append(new QLineEdit(this));
   }

    //IP地址编辑框初始化
    initIPAddrEdit();
    setFixedSize(120,25);
    QLineEdit::setReadOnly(true);
    setStyleSheet("background-color:white");

    //设置初始IP文本
    setIPText("0.0.0.0");
}

IPAddressEdit::~IPAddressEdit()
{
    nextEdit = nullptr;

    for (QLineEdit *edit : ipParts)
    {
        delete edit;
    }
    ipParts.clear();
}

void IPAddressEdit::setIPText(const QString &ipText)
{
    if (ipText.isEmpty())
    {
        for (int i = 0; i < ipParts.count(); i++)
        {
            ipParts.at(i)->setText("");
        }
        return;
    }

    //获取"."之间的IP地址
    QRegularExpression ipRegex("((\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+))");
    QRegularExpressionMatch match = ipRegex.match(ipText);

    if (match.hasMatch())
    {
        for (int i = 0; i < ipParts.count(); i++)
        {
            ipParts.at(i)->setText(match.captured(i+2));
        }
    }
}

QString IPAddressEdit::ipText() const
{
    return ipParts[0]->text() + '.' + ipParts[1]->text() + '.' + ipParts[2]->text() + '.' + ipParts[3]->text();
}

void IPAddressEdit::setPartReadOnly(bool isReadble)
{
    for (QLineEdit *ipPartEdit : ipParts)
    {
        ipPartEdit->setReadOnly(isReadble);
    }
    isPartReadOnly = isReadble;
}

bool IPAddressEdit::isPartEmpty()
{
    if ( ipText() == "...." || ipText() == "0.0.0.0")
    {
        return true;
    }
    else
    {
        return false;
    }
}

void IPAddressEdit::keyPressEvent(QKeyEvent *event)
{
    //给焦点跳转到的小编辑框设置焦点
    if(!isPartReadOnly && nextEdit)
    {
        if(event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)
        {
            //没有焦点则代替输入
            if(!nextEdit->hasFocus())
            {
                QString text = QString::number(event->key() - Qt::Key_0);  // 获取数字
                nextEdit->setText(text);
            }
            nextEdit->setFocus();
        }
        if(event->key() == Qt::Key_Backspace)
        {
            //没有焦点则代替输入
            if(!nextEdit->hasFocus())
            {
                QString text = nextEdit->text();  // 获取文本
                text.chop(1);   //去掉最后一位字符
                nextEdit->setText(text);
            }
            nextEdit->setFocus();
        }
    }
}

bool IPAddressEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (ipParts.size() > 0)
    {
        if (!isPartReadOnly)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            QFocusEvent *focusEvent = static_cast<QFocusEvent*>(event);
            QLineEdit *currentEdit = qobject_cast<QLineEdit*>(obj);

            if(currentEdit)
            {
                //焦点跳转到下一个小编辑框:右方向键、空格、还有点号键
                if ((currentEdit->cursorPosition() == currentEdit->text().size()) && (keyEvent->key() == Qt::Key_Right || keyEvent->key() == Qt::Key_Space || keyEvent->key() == Qt::Key_Period))
                {
                    return trunFocus(Qt::Key_Right, currentEdit);
                }

                 //焦点跳转到上一个小编辑框：左方向键、退位键
                if((currentEdit->cursorPosition() == 0) &&( keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Backspace))
                {
                    return trunFocus(Qt::Key_Left, currentEdit);
                }
            }

            //失去焦点则代表编辑完成
            if(focusEvent->lostFocus())
            {
                currentEdit->editingFinished();
            }
        }
    }

    return QLineEdit::eventFilter(obj, event);
}

void IPAddressEdit::initIPAddrEdit()
{
    // 设置正则表达式:只能输入三位正整数
    QRegularExpression regExp("\\d{1,3}");
    QHBoxLayout *lineEditlayout = new QHBoxLayout(this);
    lineEditlayout->setSpacing(0);
    lineEditlayout->setContentsMargins(0,0,0,0);

    for (int i = 0; i < 4; i++)
    {
        ipParts.at(i)->installEventFilter(this);
        ipParts.at(i)->setStyleSheet("border:none;background:transparent");
        ipParts.at(i)->setFixedSize(25,20);
        ipParts.at(i)->setAlignment(Qt::AlignCenter);

        QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp, ipParts.at(i));
        ipParts.at(i)->setValidator(validator);

        lineEditlayout->addWidget(ipParts.at(i));
        if(i < 3)
        {
            QLabel *dot = new QLabel(".", this);
            dot->setStyleSheet("border:none;font-weight: bold;background:transparent");
            dot->setFixedSize(5,20);
            lineEditlayout->addWidget(dot);
        }

        //超出255则输入255
        connect(ipParts.at(i), &QLineEdit::textChanged,[=](const QString &text)
        {
            if(text.toInt() > 255)
            {
                ipParts.at(i)->setText("255");
            }
        });

        connect(ipParts.at(i), &QLineEdit::editingFinished,[=]()
        {
            //无输入则输入0
            if(ipParts.at(i)->text().isEmpty())
            {
                ipParts.at(i)->setText("0");
            }

             //首字符为0则去掉首字符
            if (!ipParts.at(i)->text().isEmpty() && ipParts.at(i)->text().at(0) == '0' && ipParts.at(i)->text().length() >= 2)
            {
                ipParts.at(i)->setText(ipParts.at(i)->text().mid(1));
            }
        });
    }
}

void IPAddressEdit::setSelection(QLineEdit *curEdit)
{
    //先清空所有小编辑之前选择的文本
    foreach (QLineEdit *edit, ipParts)
    {
         edit->setSelection(0,0);
    }

    curEdit->setSelection(0,3); //选择现在跳转的编辑框文本
}

bool IPAddressEdit::trunFocus(Qt::Key key, QLineEdit *curEdit)
{
    if(!curEdit)
    {
        return false;
    }

    int curIndex = ipParts.indexOf(curEdit);
    int nextIndex = curIndex;

    switch(key)
    {
        //焦点跳转到下一个小编辑框
        case Qt::Key_Right:
            nextIndex  = curIndex + 1;
            if(curIndex == 3)
            {
                nextIndex = 0;
            }
        break;

        //焦点跳转到上一个小编辑框
        case Qt::Key_Left:
            nextIndex  = curIndex - 1;
            if(curIndex == 0)
            {
                nextIndex = 3;
            }
        break;

        default:
        break;
    }

    nextEdit = qobject_cast<QLineEdit*>(ipParts[nextIndex]);
    curEdit->clearFocus();
    setSelection(nextEdit);
    setFocus();
    return true;
}
