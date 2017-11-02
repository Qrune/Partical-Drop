#ifndef __MODEL
#define __MODEL

#include <vector>
#include <string.h>
#include "glm/glm.hpp"
#include "objload/objLoader.hpp"
using namespace std; //makes using vectors easy

#define OBJ_VEC_SIZE 3
#define POS_DIM 3
#define TEX_DIM 2
#define MAX_FACE_SIZE 3

class Material
{
public:
	
	Material() {
		texturePath = NULL;
		
		Ka = glm::vec4(0.1, 0.1, 0.1, 1.0);
		Kd = glm::vec4(0.5, 0.5, 0.5, 1.0);
		Ks = glm::vec4(0.8, 0.8, 0.8, 1.0);
		specAlpha = 10;
	}
	
	Material(Material const & m) {
		texturePath = NULL;
		*this = m;
	}
	
	Material & operator=(Material const & m) {
		if(&m == this)
			return *this;

		if(texturePath != NULL)
			free(texturePath);
		
		Ka = m.Ka;
		Kd = m.Kd;
		Ks = m.Ks;
		specAlpha = m.specAlpha;
		
		texturePath = NULL;
		
		if(m.texturePath != NULL) {
			texturePath = (char*) malloc( (strlen(m.texturePath)+1)*sizeof(char));
			strncpy(texturePath, m.texturePath, strlen(m.texturePath));
		}
        return *this;
	}
	
	~Material() {
		if(texturePath != NULL)
			free(texturePath);
	}
	
	glm::vec4 Ka;
	glm::vec4 Kd;
	glm::vec4 Ks;
	float specAlpha;
	char * texturePath;
};

class Model
{
public:
	
	void init(char const * path)
	{
		objLoader loader;
		loader.load(path);
		bool indicesMatch = true;
		
		Material defaultMaterial;
			materials.push_back(defaultMaterial);
		
		for(size_t i=0; i<loader.materialCount; i++)
		{
			Material m;
			for(size_t c=0; c<3; c++) {
				m.Ka[c] = loader.materialList[i]->amb[c];
				m.Kd[c] = loader.materialList[i]->diff[c];
				m.Ks[c] = loader.materialList[i]->spec[c];
				m.specAlpha = loader.materialList[i]->shiny;
			}
			materials.push_back(m);
		}

		positions.resize(loader.vertexCount);
		normals.resize(loader.vertexCount);
		texCoords.resize(loader.vertexCount);

		for(size_t i=0; i<normals.size(); i++)
			normals[i] = glm::vec3(0.0f);

		switchMaterialAt.push_back(0);
		
		for(size_t f=0; f<loader.faceCount; f++) {
			obj_face const * face = loader.faceList[f];
			
			int faceMaterial = face->material_index+1;
			bool firstMaterial = activeMaterial.size() == 0;
			bool materialChanged = !firstMaterial && activeMaterial[activeMaterial.size()-1] != faceMaterial;
			if(firstMaterial || materialChanged)
				activeMaterial.push_back(faceMaterial);
			
			if(!firstMaterial && materialChanged)
				switchMaterialAt.push_back(f);

			bool isNotTriangle = face->vertex_count != MAX_FACE_SIZE;
			if(isNotTriangle) {
				fprintf(stderr, "Skipping non-triangle face %zu.\n", f);
				continue;
			}

			for(size_t v=0; v<face->vertex_count; v++) {
				int pId = face->vertex_index[v];
				int nId = face->normal_index[v];
				int tId = face->texture_index[v];

				indicesMatch = indicesMatch && (pId == nId == tId);
			}

			for(size_t v=0; v<face->vertex_count; v++)
				addAttr(elements, face->vertex_index[v]);

			int pId[MAX_FACE_SIZE];
			int nId[MAX_FACE_SIZE];
			int tId[MAX_FACE_SIZE];

			glm::vec3 p[POS_DIM];
			glm::vec3 n[POS_DIM];
			glm::vec2 t[TEX_DIM];

			for(size_t v=0; v<face->vertex_count; v++) {
				pId[v] = face->vertex_index[v];
				nId[v] = face->normal_index[v];
				tId[v] = face->texture_index[v];
			}

			for(size_t v=0; v<face->vertex_count; v++) {
				for(size_t c=0; c<POS_DIM; c++) {
					p[v][c] = loader.vertexList[pId[v]]->e[c];
				}
			}

			glm::vec3 a = p[1] - p[0];
			glm::vec3 b = p[2] - p[1];
			glm::vec3 faceNormal = glm::normalize(glm::cross(a, b));

			for(size_t v=0; v<face->vertex_count; v++) {
				bool missingNormal = nId[v] < 0;
				if(missingNormal) {
					n[v] = faceNormal;
				}
				else {
					for(size_t c=0; c<POS_DIM; c++) {
						n[v][c] = loader.normalList[nId[v]]->e[c];
					}
				}
			}

			for(size_t v=0; v<face->vertex_count; v++) {
				bool validTexCoord = tId[v] >= 0;
				if(validTexCoord) {
					for(size_t c=0; c<TEX_DIM; c++) {
						t[v][c] = loader.textureList[tId[v]]->e[c];
					}
				}
			}

			for(size_t v=0; v<face->vertex_count; v++) {
				positions[ pId[v] ] = p[v];
				normals  [ pId[v] ] += n[v];
				texCoords[ pId[v] ] = t[v];
			}
		}
		
		if( !indicesMatch ) {
			fprintf(stderr, "OBJ has non-matching pos/tex/norm indices. Final model may be incorrect.\n");
		}
		
		switchMaterialAt.push_back(loader.faceCount);

		for(size_t i=0; i<normals.size(); i++)
			normals[i] = glm::normalize(normals[i]);

		min = computeMinBound();
		max = computeMaxBound();
		center = computeCentroid();
		dim = computeDimension();
        
	}
    //set different move index for different kinde of moves
    void setMoveIndex(int n){
        this->moveindex = n;
    }
    int getMoveIndex() const{
        return this->moveindex;
    }
    //every time worldstate will call move for all models
    void Move(){
        if (this->moveindex == 0){
            printf("move\n");
            return;
        }
        else{
            //move the model according to moveindex
            this->transform = glm::translate(this->transform, glm::vec3(0.01,0.01,0.01));
        }
    }
	vector<glm::vec3> const getPositions() const
	{ return positions; }
	
	vector<glm::vec3> const getNormals() const
	{ return normals; }

	vector<glm::vec2> const getTexCoords() const
	{ return texCoords; }
	
	vector<GLuint> const getElements() const
	{ return elements; }

	void setupAttributeBuffers()
	{
		glGenBuffers(3, attrBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, attrBuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, getPositionBytes(), &getPositions()[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, attrBuffer[1]);
		glBufferData(GL_ARRAY_BUFFER, getNormalBytes(), &getNormals()[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, attrBuffer[2]);
		glBufferData(GL_ARRAY_BUFFER, getTexCoordBytes(), &getTexCoords()[0], GL_STATIC_DRAW);

		glGenBuffers(1, &elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, getElementBytes(), &getElements()[0], GL_STATIC_DRAW);

		GLuint posSlot = 0;
		GLuint normSlot = 1;
		GLuint texSlot = 2;

		glGenVertexArrays(1, &vertArray);
		glBindVertexArray(vertArray);
		glBindBuffer(GL_ARRAY_BUFFER, attrBuffer[0]);
		glEnableVertexAttribArray(posSlot);
		glVertexAttribPointer(posSlot, POS_DIM, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, attrBuffer[1]);
		glEnableVertexAttribArray(normSlot);
		glVertexAttribPointer(normSlot, POS_DIM, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, attrBuffer[2]);
		glEnableVertexAttribArray(texSlot);
		glVertexAttribPointer(texSlot, TEX_DIM, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		checkGLError("model buffer");
	}

	void draw(int shaderId)
	{
		size_t KaSlot = 0;
		size_t KdSlot = 1;
		size_t KsSlot = 2;
		size_t specAlphaSlot = 3;
		
		for (size_t matGroup = 0; matGroup<switchMaterialAt.size() - 1; matGroup++)
		{
			size_t matId = activeMaterial[matGroup];
			glBindVertexArray(vertArray);
			
			glUniform4fv(glGetUniformLocation(shaderId, "Ka"), 1, &materials[matId].Ka[0]);
			glUniform4fv(glGetUniformLocation(shaderId, "Kd"), 1, &materials[matId].Kd[0]);
			glUniform4fv(glGetUniformLocation(shaderId, "Ks"), 1, &materials[matId].Ks[0]);
			glUniform4fv(glGetUniformLocation(shaderId, "specAlpha"), 1, &materials[matId].specAlpha);
			
			size_t elementCount = (switchMaterialAt[matGroup + 1] - switchMaterialAt[matGroup]);
			void* startIndex = (void*)(sizeof(GLuint) * switchMaterialAt[matGroup] * 3);
			glDrawElements(GL_TRIANGLES, elementCount*3, GL_UNSIGNED_INT, startIndex);
			glBindVertexArray(0);
		}
	}
    
	size_t getVertexCount() const
	{ return positions.size()/3; }
	
	size_t getPositionBytes() const
	{ return positions.size()*sizeof(glm::vec3); }
	
	size_t getNormalBytes() const
	{ return normals.size()*sizeof(glm::vec3); }

	size_t getTexCoordBytes() const
	{ return texCoords.size()*sizeof(glm::vec2); }
	
	size_t getElementBytes() const
	{ return elements.size()*sizeof(GLuint); }
	
	glm::vec3 getMinBound() const
	{ return min; }
	
	glm::vec3 getMaxBound() const
	{ return max; }
	
	glm::vec3 getCentroid() const
	{ return center; }
	
	glm::vec3 getDimension() const
	{ return dim; }

	glm::mat4 getTransform() const
	{ return this->transform; }

	void setTransform(glm::mat4 transform)
	{ this->transform = transform; }
	
private:
	
	glm::vec3 computeMinBound()
	{
		glm::vec3 bound;
		
		for(int c=0; c<3; c++)
			bound[c] = std::numeric_limits<float>::max();
		
		for(int i=0; i<positions.size(); i++)
		{
			for(int c=0; c<3; c++)
			{
				if(positions[i][c] < bound[c])
					bound[c] = positions[i][c];
			}
		}
		
		return bound;
	}
	
	glm::vec3 computeMaxBound()
	{
		glm::vec3 bound;
		
		for(int c=0; c<3; c++)
			bound[c] = -std::numeric_limits<float>::max();
		
		for(int i=0; i<positions.size(); i+=3)
		{
			for(int c=0; c<3; c++)
			{
				if(positions[i][c] > bound[c])
					bound[c] = positions[i][c];
			}
		}
		
		return bound;
	}

	glm::vec3 computeCentroid()
	{
		glm::vec3 center = (computeMaxBound()-computeMinBound())/2.0f + computeMinBound();
		
		return center;
	}
	
	glm::vec3 computeDimension()
	{
		glm::vec3 max = getMaxBound();
		glm::vec3 min = getMinBound();
		glm::vec3 dim = max - min;
		return dim;
	}

	template<typename Container, typename Value>
	void addAttr(Container & c, Value v)
	{
		c.push_back(v);
	}

	template<typename Container, typename Value, typename... Args>
	void addAttr(Container & c, Value v, Args... args)
	{
		c.push_back(v);
		addAttr(c, args...);
	}

	template<typename Container, typename Value>
	void setAttr(Container & c, size_t loc, Value v)
	{
		c[loc] = v;
	}

	template<typename Container, typename Value, typename... Args>
	void setAttr(Container & c, size_t loc, Value v, Args... args)
	{
		c[loc] = v;
		setAttr(c, loc+1, args...);
	}


	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::vec2> texCoords;
	vector<GLuint> elements;
	vector<Material> materials;
	vector<size_t> activeMaterial;
	vector<size_t> switchMaterialAt;
	size_t objectCount;
	
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 dim;
	glm::vec3 center;
	glm::mat4 transform;

	GLuint vertArray;
	GLuint attrBuffer[3];
	GLuint elementBuffer;
    int moveindex = 0;
    float xPos;
    float yPos;
    
    float xVel;
    float yVel;
};

#endif
