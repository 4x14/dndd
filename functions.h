#include <string.h>
#include <list>
#include "../game/render/ue.h"
#include <iostream>
#include "../Kernal/driver.hpp"

namespace uee
{
	class ue
	{
	public:
		auto get_player_name(__int64 PlayerState) -> std::string
		{
			int pNameLength;
			_WORD* pNameBufferPointer;
			int i;
			char v25;
			int v26;
			int v29;

			char16_t* pNameBuffer;

			uintptr_t pNameStructure = io::Read<uintptr_t>(PlayerState + 0xAE8);
			if (is_valid(pNameStructure)) {
				pNameLength = io::Read<int>(pNameStructure + 0x10);
				if (pNameLength <= 0)
					return "AI";

				pNameBuffer = new char16_t[pNameLength];
				uintptr_t pNameEncryptedBuffer = io::Read<uintptr_t>(pNameStructure + 0x8);
				if (is_valid(pNameEncryptedBuffer)) {
					io::ReadArray(pNameEncryptedBuffer, pNameBuffer, pNameLength);

					v25 = pNameLength - 1;
					v26 = 0;
					pNameBufferPointer = (_WORD*)pNameBuffer;

					for (i = (v25) & 3;; *pNameBufferPointer++ += i & 7) {
						v29 = pNameLength - 1;
						if (!(_DWORD)pNameLength)
							v29 = 0;

						if (v26 >= v29)
							break;

						i += 3;
						++v26;
					}

					std::u16string temp_wstring(pNameBuffer);
					delete[] pNameBuffer;
					return std::string(temp_wstring.begin(), temp_wstring.end());
				}
			}
		}

		auto in_screen(fvector2d screen_location) -> bool
		{
			if (screen_location.x > 0 && screen_location.x < screen_width && screen_location.y > 0 && screen_location.y < screen_height) return true;
			else return false;
		}

		camera_position_s get_camera()
		{
			camera_position_s camera;

			auto location_pointer = io::Read<uintptr_t>(g_ptr->uworld + 0x110);
			auto rotation_pointer = io::Read<uintptr_t>(g_ptr->uworld + 0x120);

			struct FNRot
			{
				double a; //0x0000
				char pad_0008[24]; //0x0008
				double b; //0x0020
				char pad_0028[424]; //0x0028
				double c; //0x01D0
			} fnRot;

			fnRot.a = io::Read<double>(rotation_pointer);
			fnRot.b = io::Read<double>(rotation_pointer + 0x20);
			fnRot.c = io::Read<double>(rotation_pointer + 0x1d0);

			camera.location = io::Read<fvector>(location_pointer);
			camera.rotation.x = asin(fnRot.c) * (180.0 / M_PI);
			camera.rotation.y = ((atan2(fnRot.a * -1, fnRot.b) * (180.0 / M_PI)) * -1) * -1;
			camera.fov = io::Read<float>((uintptr_t)g_ptr->player_controller + 0x394) * 90.f;

			return camera;
		}

		int dynamicfovval;

		inline fvector2d w2s(fvector WorldLocation)
		{
			camera_postion = get_camera();

			if (WorldLocation.x == 0)
				return fvector2d(0, 0);

			_MATRIX tempMatrix = Matrix(camera_postion.rotation);
			fvector vAxisX = fvector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
			fvector vAxisY = fvector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
			fvector vAxisZ = fvector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

			fvector vDelta = WorldLocation - camera_postion.location;
			fvector vTransformed = fvector(vDelta.dot(vAxisY), vDelta.dot(vAxisZ), vDelta.dot(vAxisX));

			if (vTransformed.z < 1.f)
				vTransformed.z = 1.f;

			dynamicfovval = camera_postion.fov;

			return fvector2d((screen_width / 2.0f) + vTransformed.x * (((screen_width / 2.0f) / tanf(camera_postion.fov * (float)M_PI / 360.f))) / vTransformed.z, (screen_height / 2.0f) - vTransformed.y * (((screen_width / 2.0f) / tanf(camera_postion.fov * (float)M_PI / 360.f))) / vTransformed.z);
		}

		fvector target_prediction(fvector target, fvector targetVelocity, float projectileSpeed, float projectileGravityScale, float distance)
		{
			float horizontalTime = distance / projectileSpeed;
			float verticalTime = distance / projectileSpeed;

			target.x += targetVelocity.x * horizontalTime;
			target.y += targetVelocity.y * horizontalTime;
			target.z += targetVelocity.z * verticalTime + abs(-980 * projectileGravityScale) * 0.5f * (verticalTime * verticalTime);

			return target;
		}

		static auto get_bone_3d(uintptr_t skeletal_mesh, int bone_index) -> fvector
		{
			uintptr_t bone_array = io::Read<uintptr_t>(skeletal_mesh + offset::bone_array);

			if (bone_array == NULL)

				bone_array = io::Read<uintptr_t>(skeletal_mesh + offset::bone_array + 0x10);

			FTransform bone = io::Read<FTransform>(bone_array + (bone_index * 0x60));

			FTransform component_to_world = io::Read<FTransform>(skeletal_mesh + offset::C2W);

			D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());

			return fvector(matrix._41, matrix._42, matrix._43);
		}

		static auto is_visible(uintptr_t skeletal_mesh) -> bool
		{
			auto last_submit = io::Read<float>(skeletal_mesh + 0x2E8);
			auto last_render = io::Read<float>(skeletal_mesh + 0x2F0);
			return (bool)(last_render + 0.06f >= last_submit);
		}

		auto GetProjectileSpeed(uintptr_t currentweapon) -> float
		{
			auto test = io::Read<float>(currentweapon + 0x1a60);
			return io::Read<float>(currentweapon + 0x1a60);

			std::cout << "GetProjectileSpeed -> " << test << std::endl;
			
		}
		auto GetProjectileGravity(uintptr_t currentweapon) -> float
		{
			auto test = io::Read<float>(currentweapon + 0x1ccc);
			return io::Read<float>(currentweapon + 0x1ccc);

			std::cout << "GetProjectileGravity -> " << test << std::endl;
		}

	};  static uee::ue* ue5 = new uee::ue();
}

int HeldWeaponType;

enum EFortWeaponType : int
{
	Rifle = 0,
	Shotgun = 1,
	Smg = 2,
	Sniper = 3
};

char* wchar_to_char(const wchar_t* pwchar)
{
	int currentCharIndex = 0;
	char currentChar = pwchar[currentCharIndex];

	while (currentChar != '\0')
	{
		currentCharIndex++;
		currentChar = pwchar[currentCharIndex];
	}

	const int charCount = currentCharIndex + 1;

	char* filePathC = (char*)malloc(sizeof(char) * charCount);

	for (int i = 0; i < charCount; i++)
	{
		char character = pwchar[i];

		*filePathC = character;

		filePathC += sizeof(char);

	}
	filePathC += '\0';

	filePathC -= (sizeof(char) * charCount);

	return filePathC;
}