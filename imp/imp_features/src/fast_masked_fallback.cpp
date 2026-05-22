#include <imp/features/fast_masked.hpp>

#include <fast/fast.h>
#include <imp/features/occupancy_grid_2d.hpp>

namespace ze {

void fasterCornerDetectMasked(
    const uint8_t* img, int img_width, int img_height, int img_stride,
    short barrier, const OccupancyGrid2D& grid, int level, int margin,
    std::vector<Corner>& corners)
{
  if (img_width < 7 || img_height < 7)
  {
    LOG(WARNING) << "Image too small";
    return;
  }

  std::vector<Corner> detected_corners;
  fast::fast_corner_detect_10(
        reinterpret_cast<const fast::fast_byte*>(img),
        img_width, img_height, img_stride, barrier, detected_corners);

  margin = std::max(3, margin);
  const int cell_size_log2 = grid.cellSizeLog2();
  const int max_x = img_width - margin;
  const int max_y = img_height - margin;

  for (const Corner& corner : detected_corners)
  {
    if (corner.x < margin || corner.y < margin ||
        corner.x >= max_x || corner.y >= max_y)
    {
      continue;
    }

    const int row = ((corner.y << level) >> cell_size_log2);
    const int col = ((corner.x << level) >> cell_size_log2);
    DEBUG_CHECK_LT(row, grid.n_rows_);
    DEBUG_CHECK_LT(col, grid.n_cols_);

    if (!grid.occupancy_[row * grid.n_cols_ + col])
    {
      corners.push_back(corner);
    }
  }
}

} // namespace ze
