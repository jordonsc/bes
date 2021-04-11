#ifndef BES_DBAL_WIDE_ROW_H
#define BES_DBAL_WIDE_ROW_H

#include <vector>

#include "cell.tcc"

namespace bes::dbal::wide {

struct Row
{
    std::vector<Cell> cell;
};

}  // namespace bes::dbal::wide

#endif
