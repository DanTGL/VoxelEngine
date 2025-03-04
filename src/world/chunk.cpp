#include "chunk.h"
#include "chunkmanager.h"
#include "textures.h"

#include <glm/gtc/type_ptr.hpp>
#include <noise/noise.h>

Chunk::Chunk(ChunkManager *manager) {
  this->m_pManager = manager;
  // Create the blocks
  m_pBlocks = new Block **[CHUNK_SIZE];
  for (int i = 0; i < CHUNK_SIZE; i++) {
    m_pBlocks[i] = new Block *[WORLD_HEIGHT];

    for (int j = 0; j < WORLD_HEIGHT; j++) {
      m_pBlocks[i][j] = new Block[CHUNK_SIZE];
    }
  }
  Texture t;
  GLuint texture = loadTextureArray("assets/textures/atlas.bmp");

  t.id = texture;

  mesh.textures.push_back(t);

  m_emptyChunk = false;
  m_loaded = false;
  m_setup = false;

  // SetupLandscape();
}

Chunk::~Chunk() {
  // Delete the blocks
  for (int i = 0; i < CHUNK_SIZE; ++i) {
    for (int j = 0; j < WORLD_HEIGHT; ++j) {
      delete[] m_pBlocks[i][j];
    }
    delete[] m_pBlocks[i];
  }
  delete[] m_pBlocks;
}

void Chunk::Render(Shader shader) {
  // TODO: Figure out why I added this line:
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  if (!mesh.vertices.empty()) {
    glm::mat4 model = glm::mat4(1.0f);
    model =
        glm::rotate(model, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(GetX() * Chunk::CHUNK_SIZE, -20,
                                            GetZ() * Chunk::CHUNK_SIZE));

    int modelLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    mesh.setupMesh();

    mesh.Render(shader);
  }
}

void Chunk::CreateMesh() {
  mesh.indices.clear();
  mesh.vertices.clear();
  bool lDefault = true;

  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < WORLD_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        if (m_pBlocks[x][y][z].IsActive() == false) {
          // Don't create triangle data for inactive blocks
          continue;
        }

        bool lXNegative = lDefault;
        if (x > 0) {
          lXNegative = m_pBlocks[x - 1][y][z].IsActive();
        }

        if (x == 0 && this->m_pManager->GetChunk(this->GetX() - 1,
                                                 this->GetZ()) != NULL) {
          lXNegative =
              this->m_pManager->GetChunk(this->GetX() - 1, this->GetZ())
                  ->m_pBlocks[CHUNK_SIZE - 1][y][z]
                  .IsActive();
        }

        if (x == 0) {
          lXNegative = false;
        }

        bool lXPositive = lDefault;
        if (x < CHUNK_SIZE - 1) {
          lXPositive = m_pBlocks[x + 1][y][z].IsActive();
        }

        if (x == CHUNK_SIZE - 1 &&
            this->m_pManager->GetChunk(this->GetX() + 1, this->GetZ()) !=
                NULL) {
          lXPositive =
              this->m_pManager->GetChunk(this->GetX() + 1, this->GetZ())
                  ->m_pBlocks[0][y][z]
                  .IsActive();
        }

        if (x == CHUNK_SIZE - 1) {
          lXPositive = false;
        }

        bool lYNegative = lDefault;
        if (y > 0)
          lYNegative = m_pBlocks[x][y - 1][z].IsActive();

        bool lYPositive = lDefault;
        if (y < WORLD_HEIGHT - 1)
          lYPositive = m_pBlocks[x][y + 1][z].IsActive();

        bool lZNegative = lDefault;
        if (z > 0) {
          lZNegative = m_pBlocks[x][y][z - 1].IsActive();
        }

        if (z == 0 && this->m_pManager->GetChunk(this->GetX(),
                                                 this->GetZ() - 1) != NULL) {
          lZNegative =
              this->m_pManager->GetChunk(this->GetX(), this->GetZ() - 1)
                  ->m_pBlocks[x][y][CHUNK_SIZE - 1]
                  .IsActive();
        }

        if (z == 0) {
          lZNegative = false;
        }

        bool lZPositive = lDefault;
        if (z < CHUNK_SIZE - 1) {
          lZPositive = m_pBlocks[x][y][z + 1].IsActive();
        }

        if (z == CHUNK_SIZE - 1 &&
            this->m_pManager->GetChunk(this->GetX(), this->GetZ() + 1) !=
                NULL) {
          lZPositive =
              this->m_pManager->GetChunk(this->GetX(), this->GetZ() + 1)
                  ->m_pBlocks[x][y][0]
                  .IsActive();
        }

        if (z == CHUNK_SIZE - 1) {
          lZPositive = false;
        }

        CreateCube(x, y, z, lXNegative, lXPositive, lYNegative, lYPositive,
                   lZNegative, lZPositive);
      }
    }
  }

  mesh.setupMesh();
}

void Chunk::CreateCube(float x, float y, float z, bool xNegative,
                       bool xPositive, bool yNegative, bool yPositive,
                       bool zNegative, bool zPositive) {
  glm::vec3 p1(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE,
               z + BLOCK_RENDER_SIZE);
  glm::vec3 p2(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE,
               z + BLOCK_RENDER_SIZE);
  glm::vec3 p3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE,
               z + BLOCK_RENDER_SIZE);
  glm::vec3 p4(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE,
               z + BLOCK_RENDER_SIZE);
  glm::vec3 p5(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE,
               z - BLOCK_RENDER_SIZE);
  glm::vec3 p6(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE,
               z - BLOCK_RENDER_SIZE);
  glm::vec3 p7(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE,
               z - BLOCK_RENDER_SIZE);
  glm::vec3 p8(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE,
               z - BLOCK_RENDER_SIZE);

  glm::vec3 n1;

  unsigned int v1, v2, v3, v4, v5, v6, v7, v8;

  glm::vec2 texCoord1(0.0f, 0.0f);
  glm::vec2 texCoord2(1.0f, 0.0f);
  glm::vec2 texCoord3(1.0f, 1.0f);
  glm::vec2 texCoord4(0.0f, 1.0f);

  // Front

  if (!zPositive) {

    n1 = glm::vec3(0.0f, 0.0f, 1.0f);

    v1 = mesh.AddVertex(
        p1, n1, texCoord1,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_FRONT]);
    v2 = mesh.AddVertex(
        p2, n1, texCoord2,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_FRONT]);
    v3 = mesh.AddVertex(
        p3, n1, texCoord3,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_FRONT]);
    v4 = mesh.AddVertex(
        p4, n1, texCoord4,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_FRONT]);

    mesh.AddTriangle(v1, v2, v3);
    mesh.AddTriangle(v1, v3, v4);
  }

  // Back

  if (!zNegative) {
    n1 = glm::vec3(0.0f, 0.0f, -1.0f);

    v5 = mesh.AddVertex(
        p5, n1, texCoord1,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_BACK]);
    v6 = mesh.AddVertex(
        p6, n1, texCoord2,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_BACK]);
    v7 = mesh.AddVertex(
        p7, n1, texCoord3,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_BACK]);
    v8 = mesh.AddVertex(
        p8, n1, texCoord4,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_BACK]);

    mesh.AddTriangle(v5, v6, v7);
    mesh.AddTriangle(v5, v7, v8);
  }

  // Right

  if (!xPositive) {
    n1 = glm::vec3(1.0f, 0.0f, 0.0f);

    v2 = mesh.AddVertex(
        p2, n1, texCoord1,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_RIGHT]);
    v5 = mesh.AddVertex(
        p5, n1, texCoord2,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_RIGHT]);
    v8 = mesh.AddVertex(
        p8, n1, texCoord3,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_RIGHT]);
    v3 = mesh.AddVertex(
        p3, n1, texCoord4,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_RIGHT]);

    mesh.AddTriangle(v2, v5, v8);
    mesh.AddTriangle(v2, v8, v3);
  }

  // Left
  if (!xNegative) {
    n1 = glm::vec3(-1.0f, 0.0f, 0.0f);

    v6 = mesh.AddVertex(
        p6, n1, texCoord1,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_LEFT]);
    v1 = mesh.AddVertex(
        p1, n1, texCoord2,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_LEFT]);
    v4 = mesh.AddVertex(
        p4, n1, texCoord3,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_LEFT]);
    v7 = mesh.AddVertex(
        p7, n1, texCoord4,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_LEFT]);

    mesh.AddTriangle(v6, v1, v4);
    mesh.AddTriangle(v6, v4, v7);
  }

  // Top

  if (!yPositive) {
    n1 = glm::vec3(0.0f, 1.0f, 0.0f);

    v4 = mesh.AddVertex(
        p4, n1, texCoord1,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_TOP]);
    v3 = mesh.AddVertex(
        p3, n1, texCoord2,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_TOP]);
    v8 = mesh.AddVertex(
        p8, n1, texCoord3,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_TOP]);
    v7 = mesh.AddVertex(
        p7, n1, texCoord4,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_TOP]);

    mesh.AddTriangle(v4, v3, v8);
    mesh.AddTriangle(v4, v8, v7);
  }

  // Bottom

  if (!yNegative) {
    n1 = glm::vec3(0.0f, -1.0f, 0.0f);

    v6 = mesh.AddVertex(
        p6, n1, texCoord1,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_BOTTOM]);
    v5 = mesh.AddVertex(
        p5, n1, texCoord2,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_BOTTOM]);
    v2 = mesh.AddVertex(
        p2, n1, texCoord3,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_BOTTOM]);
    v1 = mesh.AddVertex(
        p1, n1, texCoord4,
        textures[m_pBlocks[(int)x][(int)y][(int)z].m_blockType][FACE_BOTTOM]);

    mesh.AddTriangle(v6, v5, v2);
    mesh.AddTriangle(v6, v2, v1);
  }
}

void Chunk::UpdateShouldRenderFlags() {
  // Figure out if we are a completely empty chunk
  size_t numVerts;
  size_t numTriangles;

  mesh.GetMeshInformation(numVerts, numTriangles);

  if (numVerts == 0 && numTriangles == 0) {
    m_emptyChunk = true;
  } else {
    m_emptyChunk = false;
  }
}

bool Chunk::ShouldRender() { return !m_emptyChunk; }

bool Chunk::IsLoaded() { return m_loaded; }

bool Chunk::IsSetup() { return m_setup; }

void Chunk::Load() { this->m_loaded = true; }

void Chunk::Unload() { this->m_loaded = false; }

void Chunk::Setup() { this->m_setup = true; }

void Chunk::Update() {}

void Chunk::SetPosition(glm::vec2 pos) { worldPos = pos; }

void Chunk::SetGrid(int gridX, int gridZ) {
  gridPos.x = gridX;
  gridPos.y = gridZ;
}

int Chunk::GetX() { return gridPos.x; }

int Chunk::GetZ() { return gridPos.y; }

glm::vec2 Chunk::GetPosition() { return worldPos; }

void Chunk::SetupLandscape(ImprovedCombinedNoise *noise1,
                           ImprovedCombinedNoise *noise2,
                           ImprovedOctaveNoise *noise3) {

  for (int z = 0; z < CHUNK_SIZE; z++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      double heightResult = 0;
      double heightLow = noise1->compute(((gridPos.x * CHUNK_SIZE) + x) * 1.3,
                                         ((gridPos.y * CHUNK_SIZE) + z) * 1.3) /
                             6 -
                         4;
      double heightHigh =
          noise2->compute(((gridPos.x * CHUNK_SIZE) + x) * 1.3,
                          ((gridPos.y * CHUNK_SIZE) + z) * 1.3) /
              5 +
          6;

      if (noise3->compute((gridPos.x * CHUNK_SIZE) + x,
                          (gridPos.y * CHUNK_SIZE) + z) /
              8 >
          0) {
        heightResult = heightLow;
      } else {
        heightResult = std::fmax(heightLow, heightHigh);
      }

      heightResult *= 0.5;

      if (heightResult < 0) {
        heightResult *= 0.8f;
      }

      float height = heightResult + 32;
      int dirtThickness = noise1->compute((gridPos.x * CHUNK_SIZE) + x,
                                          (gridPos.y * CHUNK_SIZE) + z) /
                              24 -
                          4;
      double dirtTransition = height;
      double stoneTransition = dirtTransition + dirtThickness;

      for (int y = 0; y < height; y++) {
        BlockType type = BlockType_Default;

        if (y == 0)
          type = BlockType_Stone;
        else if (y <= stoneTransition)
          type = BlockType_Stone;
        else if (y <= dirtTransition)
          type = BlockType_Grass;
        if (type != BlockType_Default) {
          m_pBlocks[x][y][z].SetActive(true);
          m_pBlocks[x][y][z].m_blockType = type;
        }
      }
    }
  }
}
