#include "main.hh"

Block::Block(bool isData, quint64 idx, QByteArray data)
{
    this->isData = isData;
    this->idx = idx;
    this->data = data;
}
