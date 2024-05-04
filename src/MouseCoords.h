#pragma once

#pragma pack(push, 4)
class MouseCoords
{
public:
	static MouseCoords* GetSingleton()
	{
		REL::Relocation<MouseCoords*> g_mouseCoords(0x02FEBC40);
		return g_mouseCoords.get();
	}

	// members
	std::uint32_t unk00;  // 00
	float unk04;          // 04
	float x;              // 08
	float y;              // 0C
	float minX;           // 10
	float minY;           // 14
	float maxX;           // 18 - menu texture resolution I think
	float maxY;           // 1C - menu texture resolution I think
	float unk20;          // 20
	float unk24;          // 24
	float unk28;          // 28
	float unk2C;          // 2C
	float unk30;          // 30
};
#pragma pack(pop)
static_assert(sizeof(MouseCoords) == 0x34);
