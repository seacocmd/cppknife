/*
 * SvgTool.cpp
 *
 *  Created on: 17.06.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "tools.hpp"

namespace cppknife {

SvgTool::SvgTool(size_t startSize, size_t clusterSize, size_t capacity) :
    _color("black"), _colorText("black"), _strokeWidth(1), _output(startSize,
        clusterSize, capacity), _xFactor(1.0) {
}

SvgTool::~SvgTool() {
}

SvgTool::SvgTool(const SvgTool &other) :
    _color(other._color), _colorText(other._colorText), _strokeWidth(
        other._strokeWidth), _output(other._output.startSize(),
        other._output.clusterSize(), other._output.capacity()), _xFactor(
        other._xFactor) {
}

SvgTool&
SvgTool::operator=(const SvgTool &other) {
  _color = other._color;
  _colorText = other._colorText;
  _strokeWidth = other._strokeWidth;
  _output = other._output;
  _xFactor = other._xFactor;
  return *this;
}

void SvgTool::log(const char *message) {
  printf("%s\n", message);
}

void SvgTool::htmlEnd() {
  _output.append("</body>\n</html>");
}

void SvgTool::htmlStart(const char *title) {
  char buffer[4096];
  _output.append(
      joinCStrings(buffer, sizeof buffer, "<html>\n<body>\n<h1>", title,
          "</h1>\n"));
}

void SvgTool::multiline(std::vector<double> coordinates, const char *color,
    const char *properties) {
  size_t count = coordinates.size();
  for (size_t ix = 0; ix < (count - 1) / 2; ix++) {
    simpleLine(coordinates[2 * ix], coordinates[2 * ix + 1],
        coordinates[2 + 2 * ix], coordinates[2 + 2 * ix + 1], color,
        properties);
  }
}

void SvgTool::point(float x, float y, float width, const char *text,
    const char *color, const char *properties) {
  auto half = width / _xFactor / 2;
  simpleLine(x - half, y - half, x + half, y + half, properties, color);
  simpleLine(x - half, y + half, x + half, y - half, properties, color);
  if (text != nullptr) {
    simpleText(x + half, y, text, _colorText);
  }
}

void SvgTool::scaleBy(SheetData &sheet, float xMin, float yMin, float width,
    float height, int widthPixel, int border) {
  auto scale = (widthPixel - 2 * border) / width;
  auto yMax = yMin + height;
  setScale(scale, -scale, border - xMin * scale, border + yMax * scale);
  //@ToDo
  //return (widthPixel, int(height * scale + 2*border));

}

void SvgTool::simpleLine(float x1, float y1, float x2, float y2,
    const char *color, const char *properties) {
}

void SvgTool::setScale(float xFactor, float yFActor, float xOffset,
    float yOffset) {
}

void SvgTool::simpleText(float x, float y, const char *text, const char *color,
    const char *properties) {
}

void SvgTool::svgEnd() {
}

void SvgTool::svgStart(int width, int height) {
}

void SvgTool::write(const char *filename) {
}

} /* namespace polygeo */
