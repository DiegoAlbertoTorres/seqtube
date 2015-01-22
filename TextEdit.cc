#include "main.hh"

TextEdit::TextEdit()
{
	setTabChangesFocus(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum); // Horizontal, Vertical
	setFixedHeight(sizeHint().height());
}

void TextEdit::keyPressEvent(QKeyEvent *event)
{
	if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && event->modifiers() == Qt::SHIFT){
		QKeyEvent *e = new QKeyEvent(event->type(), event->key(), Qt::NoModifier);
		QPlainTextEdit::keyPressEvent(e);
	}
	else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
		emit returnPressed();
	}
	else
		QPlainTextEdit::keyPressEvent(event);
}

QSize TextEdit::sizeHint()
{
	QFontMetrics fm(font());
	int h = (2 * qMax(fm.height(), 14)) + 6;
	int w = fm.width(QLatin1Char('x')) * 17 + 4;
	QStyleOptionFrameV2 opt;
	opt.initFrom(this);
	return (style()->sizeFromContents(QStyle::CT_LineEdit, &opt, 
		QSize(w, h).expandedTo(QApplication::globalStrut()), this));
}
