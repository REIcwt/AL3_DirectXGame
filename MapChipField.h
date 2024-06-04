#pragma once
#include "DirectXCommon.h"
#include <vector>
#include <string>
#include<fstream>
#include <sstream>
#include <Vector3.h>

enum class MapChipType {
	kBlank,
	kBlock,
};

struct  MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:

	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; };
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; };

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex){
		Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

		Rect rect;
		rect.left = center.x - 0;
		rect.right = center.x + 0;
		rect.bottom = center.y - kBlockHeight / 2.0f;
		rect.top = center.y + kBlockHeight / 2.0f;

		return rect;
	};

private:
	// block size
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;

	// block number
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;

};