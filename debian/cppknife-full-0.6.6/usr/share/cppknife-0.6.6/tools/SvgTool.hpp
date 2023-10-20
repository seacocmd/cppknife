/*
 * SvgTool.hpp
 *
 *  Created on: 17.06.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef SVGTOOL_HPP_
#define SVGTOOL_HPP_

namespace cppknife {
/**
 * Stores the position data of a "sheet".
 */
struct SheetData {
  int widthPixel;
  int heightPixel;
};
/**
 * @brief Manages a Scalable Vector Graphic (SVG) image in a HTML file.
 */
class SvgTool final {
protected:
  const char *_color;
  const char *_colorText;
  int _strokeWidth;
  float _xFactor;
public:
  /**
   * Constructor.
   * @param startSize The initial capacity of the line buffer.
   * @param clusterSize If the capacity of the line buffer is reached that amount of entries
   * 	will be added.
   * 	@param capacity The capacity of the string buffer.
   */
  SvgTool(size_t startSize = 100, size_t clusterSize = 100, size_t capacity =
      0x10000);
  virtual
  ~SvgTool();
  SvgTool(const SvgTool &other);
  SvgTool&
  operator=(const SvgTool &other);
public:
  void
  log(const char *message);
  void
  htmlStart(const char *title);
  void
  htmlEnd();
  void
  multiline(std::vector<double> coordinates, const char *color = nullptr,
      const char *properties = nullptr);
  void
  point(float x, float y, float width, const char *text = nullptr,
      const char *color = nullptr, const char *properties = nullptr);
  void
  scaleBy(SheetData &sheet, float xMin, float yMin, float width, float height,
      int widthPixel = 800, int border = 20);
  void
  simpleLine(float x1, float y1, float x2, float y2,
      const char *color = nullptr, const char *properties = nullptr);
  void
  setScale(float xFactor, float yFActor, float xOffset, float yOffset);
  void
  simpleText(float x, float y, const char *text, const char *color = nullptr,
      const char *properties = nullptr);
  void
  svgEnd();
  void
  svgStart(int width, int height);
  void
  write(const char *filename);

};

} /* namespace polygeo */

#endif /* SVGTOOL_HPP_ */
