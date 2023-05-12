#include "OBJModel.hpp"
#include "../Utility/Utility.hpp"

#include <cassert>
#include <iostream>
#include <fstream>

namespace PathTracer {
	void OBJModel::LoadObjFile(const std::string& fileName)
	{
		std::ifstream file("Assets/" + fileName);
		if (!file) {
			std::cout << "�t�@�C���I�[�v���Ɏ��s���܂����B" << std::endl;
			std::cout << "fileName: " << fileName << std::endl;
		}
		else {
			std::cout << "�t�@�C��: " << fileName << "���I�[�v�����܂����B" << std::endl;
		}

		// �t�@�C�����烂�f������ǂݍ���
		std::string str;
		std::vector<std::string> list;

		struct Surface {
			std::vector<size_t> vertexIndices;
			std::vector<size_t> normalIndices;
		};

		std::vector<Surface> faces = std::vector<Surface>();

		while (std::getline(file, str))
		{
			// �w�b�_or��s����
			if (str[0] == '#' || str.empty()) {
				continue;
			}
			if (str[0] == 'f') {
				// �����񕪊�
				list = SplitString(str, " ");
			}
			else {
				// �����񕪊�
				list = SplitString(str, " ");
			}


			// ���_
			if (list[0] == "v") {
				m_vertices.push_back(
					Vector3(
						std::stof(list[1]),
						std::stof(list[2]),
						std::stof(list[3]))
				);
			}
			// ���_�@��
			else if (list[0] == "vn") {
				m_normals.push_back(
					Vector3(
						std::stof(list[1]),
						std::stof(list[2]),
						std::stof(list[3]))
				);
			}
			// �ʏ��̉��
			else if (list[0] == "f") {
				m_normals.shrink_to_fit();
				m_vertices.shrink_to_fit();
				Surface face;
				face.vertexIndices.resize(list.size() - 1);
				face.normalIndices.resize(list.size() - 1);

				for (size_t i = 1; i < list.size(); i++) {
					// �t�H�[�}�b�g����
					// ���_�C���f�b�N�X//���_�@��
					if (list[i].find("//") != std::string::npos) {
						// ���_�C���f�b�N�X//�@���C���f�b�N�X�̏ꍇ��z��
						std::vector<std::string> subLists = SplitString(list[i], "//");

						// TODO: UV�͕K�v�ɂȂ�������
						// �C���f�b�N�X�ԍ���1�n�܂�ƂȂ��Ă���̂ŁC�z��͈̔͊O�Q�Ƃ�h�����߂�-1����
						face.vertexIndices[i - 1] = static_cast<unsigned int>(std::stoi(subLists[0])) - 1;
						face.normalIndices[i - 1] = static_cast<unsigned int>(std::stoi(subLists[1])) - 1;
					}
					else if (list[i].find("/") != std::string::npos) {
						std::vector<std::string> subLists = SplitString(list[i], "/");

						// ���_�C���f�b�N�X/UV�C���f�b�N�X
						if (subLists.size() == 2) {
							// TODO: ���_����@�������̎���
							std::cout << "��Ή��`���ł��B" << std::endl;
							assert(true);
						}

						// ���_�C���f�b�N�X/UV�C���f�b�N�X/�@���C���f�b�N�X
						// TODO: UV�͕K�v�ɂȂ�������
						// �C���f�b�N�X�ԍ���1�n�܂�ƂȂ��Ă���̂ŁC�z��͈̔͊O�Q�Ƃ�h�����߂�-1����
						face.vertexIndices[i - 1] = static_cast<unsigned int>(std::stoi(subLists[0])) - 1;
						face.normalIndices[i - 1] = static_cast<unsigned int>(std::stoi(subLists[2])) - 1;
					}
					// ���_�̂�
					else {
						std::cout << "��Ή��`���ł��B" << std::endl;
						assert(true);
					}
				}
				faces.push_back(face);
			}
		}
		file.close();

		// �@����񂪂Ȃ��ꍇ�͒��_�@�����쐬����
		std::vector<Vector3> surfaceNormals(faces.size(), Vector3());
		if (m_normals.empty()) {
			for (size_t i = 0; i < m_vertices.size(); i++) {
				std::vector<Surface> shareFaces = std::vector<Surface>();

				// ���_�����L����ʂ����X�g�A�b�v
				for (auto& face : faces) {
					if (face.vertexIndices[0] == i || face.vertexIndices[1] == i || face.vertexIndices[2] == i) {
						shareFaces.push_back(face);
					}
				}

				// ���X�g�A�b�v�����ʂ̖ʖ@�������߂�
				Vector3 vertexNormal = Vector3();
				size_t index = 0;
				for (auto face : shareFaces) {
					if (surfaceNormals[index] == Vector3()) {
						const std::vector<Vector3> vertex = { m_vertices[face.vertexIndices[0]], m_vertices[face.vertexIndices[1]], m_vertices[face.vertexIndices[2]] };
						vertexNormal += Normalize(Cross(vertex[1] - vertex[0], vertex[2] - vertex[0]));
					}
					else {
						vertexNormal += surfaceNormals[index];
					}
					index++;
				}

				// �ʖ@���̕��ϒl�����_�@���ƂȂ�
				vertexNormal /= shareFaces.size();
				m_normals.push_back(Normalize(vertexNormal));

			}

			// �@���̃C���f�b�N�X�f�[�^����
			for (auto& face : faces) {
				face.normalIndices = face.vertexIndices;
			}
		}


		// �|���S���𐶐�
		m_polygons.resize(faces.size());
		size_t polygonIndex = 0;
		for (auto& face : faces) {
			m_polygons[polygonIndex] = Polygon(
				m_vertices[face.vertexIndices[0]], m_vertices[face.vertexIndices[1]], m_vertices[face.vertexIndices[2]],
				m_normals[face.normalIndices[0]], m_normals[face.normalIndices[1]], m_normals[face.normalIndices[2]],
				m_objectID, m_triangleMask);
			polygonIndex++;
		}
	}
}