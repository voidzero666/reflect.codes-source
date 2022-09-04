
#include "Hooks.h"
#include "predictioncopy.h"



PredictionCopy::PredictionCopy(int type, uint8_t* dest, bool dest_packed, const uint8_t* src, bool src_packed, optype_t opType, FN_FIELD_COMPARE func) noexcept
{
	this->opType = opType;
	this->type = type;
	this->dest = dest;
	this->src = src;
	destOffsetIndex = dest_packed ? TD_OFFSET_PACKED : TD_OFFSET_NORMAL;
	srcOffsetIndex = src_packed ? TD_OFFSET_PACKED : TD_OFFSET_NORMAL;

	errorCount = 0;
	entIndex = -1;

	watchField = NULL;
	fieldCompareFunc = func;
}

int PredictionCopy::TransferData(const char* operation, int entindex, datamap_t* dmap) noexcept
{
	return csgo->transferData(this, operation, entindex, dmap);
}