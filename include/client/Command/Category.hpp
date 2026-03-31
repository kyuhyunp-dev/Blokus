#ifndef CATEGORY_HPP
#define CATEGORY_HPP

namespace Category
{
    enum Type
    {
        None = 0,
        ActivePiece = 1 << 0,
        Board = 1 << 1,
        Arena = 1 << 2,
    };
};

#endif