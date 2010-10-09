/* This file is part of Clementine.

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "collapsibleinfoheader.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

const int CollapsibleInfoHeader::kHeight = 20;
const int CollapsibleInfoHeader::kIconSize = 16;

CollapsibleInfoHeader::CollapsibleInfoHeader(QWidget* parent)
  : QWidget(parent),
    expanded_(true),
    hovering_(false)
{
  setMinimumHeight(kHeight);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setCursor(QCursor(Qt::PointingHandCursor));
}

void CollapsibleInfoHeader::SetTitle(const QString& title) {
  title_ = title;
  update();
}

void CollapsibleInfoHeader::SetIcon(const QIcon& icon) {
  icon_ = icon;
  update();
}

void CollapsibleInfoHeader::SetExpanded(bool expanded) {
  expanded_ = expanded;

  emit ExpandedToggled(expanded);
  if (expanded)
    emit Expanded();
  else
    emit Collapsed();
}

void CollapsibleInfoHeader::enterEvent(QEvent*) {
  hovering_ = true;
  update();
}

void CollapsibleInfoHeader::leaveEvent(QEvent*) {
  hovering_ = false;
  update();
}

void CollapsibleInfoHeader::paintEvent(QPaintEvent* e) {
  QPainter p(this);

  QRect indicator_rect(0, 0, height(), height());
  QRect icon_rect(height() + 2, (kHeight - kIconSize) / 2, kIconSize, kIconSize);
  QRect text_rect(rect());
  text_rect.setLeft(icon_rect.right() + 4);

  // Draw the background
  const QColor bg_color_1(palette().color(QPalette::Highlight).lighter(120));
  const QColor bg_color_2(palette().color(QPalette::Highlight).darker(120));
  const QColor bg_border(palette().color(QPalette::Dark));
  QLinearGradient bg_brush(rect().topLeft(), rect().bottomLeft());
  bg_brush.setColorAt(0.0, bg_color_1);
  bg_brush.setColorAt(0.5, bg_color_1);
  bg_brush.setColorAt(0.5, bg_color_2);
  bg_brush.setColorAt(1.0, bg_color_2);

  p.setPen(Qt::NoPen);
  p.fillRect(rect(), bg_brush);

  p.setPen(bg_border);
  p.drawLine(rect().topLeft(), rect().topRight());
  p.drawLine(rect().bottomLeft(), rect().bottomRight());

  // Draw the expand/collapse indicator
  QStyleOption opt;
  opt.initFrom(this);
  opt.rect = indicator_rect;
  opt.state |= QStyle::State_Children;
  if (expanded_)
    opt.state |= QStyle::State_Open;
  if (hovering_)
    opt.state |= QStyle::State_Active;

  // Have to use the application's style here because using the widget's style
  // will trigger QStyleSheetStyle's recursion guard (I don't know why).
  QApplication::style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt, &p, this);

  // Draw the icon
  p.drawPixmap(icon_rect, icon_.pixmap(kIconSize));

  // Draw the title text
  QFont bold_font(font());
  bold_font.setBold(true);
  p.setFont(bold_font);

  p.setPen(palette().color(QPalette::HighlightedText));
  p.drawText(text_rect, Qt::AlignLeft | Qt::AlignVCenter, title_);
}

void CollapsibleInfoHeader::mouseReleaseEvent(QMouseEvent* e) {
  SetExpanded(!expanded_);
}
