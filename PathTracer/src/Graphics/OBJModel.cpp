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
			std::cout << "ファイルオープンに失敗しました。" << std::endl;
			std::cout << "fileName: " << fileName << std::endl;
		}
		else {
			std::cout << "ファイル: " << fileName << "をオープンしました。" << std::endl;
		}

		// ファイルからモデル情報を読み込む
		std::string str;
		std::vector<std::string> list;

		struct Surface {
			std::vector<size_t> vertexIndices;
			std::vector<size_t> normalIndices;
		};

		std::vector<Surface> faces = std::vector<Surface>();

		while (std::getline(file, str))
		{
			// ヘッダor空行判定
			if (str[0] == '#' || str.empty()) {
				continue;
			}
			if (str[0] == 'f') {
				// 文字列分割
				list = SplitString(str, " ");
			}
			else {
				// 文字列分割
				list = SplitString(str, " ");
			}


			// 頂点
			if (list[0] == "v") {
				m_vertices.push_back(
					Vector3(
						std::stof(list[1]),
						std::stof(list[2]),
						std::stof(list[3]))
				);
			}
			// 頂点法線
			else if (list[0] == "vn") {
				m_normals.push_back(
					Vector3(
						std::stof(list[1]),
						std::stof(list[2]),
						std::stof(list[3]))
				);
			}
			// 面情報の解析
			else if (list[0] == "f") {
				m_normals.shrink_to_fit();
				m_vertices.shrink_to_fit();
				Surface face;
				face.vertexIndices.resize(list.size() - 1);
				face.normalIndices.resize(list.size() - 1);

				for (size_t i = 1; i < list.size(); i++) {
					// フォーマット判定
					// 頂点インデックス//頂点法線
					if (list[i].find("//") != std::string::npos) {
						// 頂点インデックス//法線インデックスの場合を想定
						std::vector<std::string> subLists = SplitString(list[i], "//");

						// TODO: UVは必要になったらやる
						// インデックス番号は1始まりとなっているので，配列の範囲外参照を防ぐために-1する
						face.vertexIndices[i - 1] = static_cast<unsigned int>(std::stoi(subLists[0])) - 1;
						face.normalIndices[i - 1] = static_cast<unsigned int>(std::stoi(subLists[1])) - 1;
					}
					else if (list[i].find("/") != std::string::npos) {
						std::vector<std::string> subLists = SplitString(list[i], "/");

						// 頂点インデックス/UVインデックス
						if (subLists.size() == 2) {
							// TODO: 頂点から法線生成の実装
							std::cout << "非対応形式です。" << std::endl;
							assert(true);
						}

						// 頂点インデックス/UVインデックス/法線インデックス
						// TODO: UVは必要になったらやる
						// インデックス番号は1始まりとなっているので，配列の範囲外参照を防ぐために-1する
						face.vertexIndices[i - 1] = static_cast<unsigned int>(std::stoi(subLists[0])) - 1;
						face.normalIndices[i - 1] = static_cast<unsigned int>(std::stoi(subLists[2])) - 1;
					}
					// 頂点のみ
					else {
						std::cout << "非対応形式です。" << std::endl;
						assert(true);
					}
				}
				faces.push_back(face);
			}
		}
		file.close();

		// 法線情報がない場合は頂点法線を作成する
		std::vector<Vector3> surfaceNormals(faces.size(), Vector3());
		if (m_normals.empty()) {
			for (size_t i = 0; i < m_vertices.size(); i++) {
				std::vector<Surface> shareFaces = std::vector<Surface>();

				// 頂点を共有する面をリストアップ
				for (auto& face : faces) {
					if (face.vertexIndices[0] == i || face.vertexIndices[1] == i || face.vertexIndices[2] == i) {
						shareFaces.push_back(face);
					}
				}

				// リストアップした面の面法線を求める
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

				// 面法線の平均値が頂点法線となる
				vertexNormal /= shareFaces.size();
				m_normals.push_back(Normalize(vertexNormal));

			}

			// 法線のインデックスデータ生成
			for (auto& face : faces) {
				face.normalIndices = face.vertexIndices;
			}
		}


		// ポリゴンを生成
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