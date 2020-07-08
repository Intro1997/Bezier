#pragma once
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class Bezier {
public:
	// ȷ�����������ߵĽ�����һ�ף����ף����ף�
	static enum class OrderType {
		FirstOrder = 0, SecondOrder = 1, ThirdOrder = 2
	};
private:
	// ���� OpenGL ��Ⱦ�� Vertex Array Object
	std::vector<unsigned int> VAOs;
	unsigned int ControlPointVAO;
	
	// ���� OpenGL ��Ⱦ�� shader ·��
	const char* LineShaderPath = "src/shader/Line.shader";	
	const char* ControlShaderPath = "src/shader/ControlPoint.shader";

	// �洢�û����빹�캯���ĸ����ڵ����ݣ����ڻ��ƿ��Ʊ��������߻��ƵĽڵ�
	std::vector<float>* control_point_data;
	unsigned int control_point_cnt;

	// ��ȡ GLFW window ���󣬴Ӷ���ȡָ��λ�ã�����ʹ��ָ��������Ƶ㹦��
	GLFWwindow *WINDOW;

	// OpenGL shader
	Shader LineShader;
	Shader ControlPointShader;

	// ��¼�����һ�ε�����λ��
	float lastXpos;
	float lastYpos;
	unsigned int current_selected_control_point_index;
	

	/**
	 * ���ڹ������������ߵĲ���
	 *		1. LINE_RESOLUTION			��ȷ�������ɶ�����ֱ�߻��ƶ��ɣ�ȷ�����ߵľ�ϸ�̶ȣ������ 10 ����Ĭ��Ϊ 10 ��
	 *		2. DIMENSION				��ȷ����Ҫ����������ά�ȣ�Ŀǰ��֧�� 2 ά
	 *		3. LINE_WIDTH				��ȷ�����ߵĿ�ȣ�Ĭ��Ϊ 3
	 *		4. DRAW_CONTROL_POINT		: ȷ���Ƿ���ƿ��Ƶ�
	 *		5. CONTROL_POINT_SIZE		��ȷ�����Ƶ�ĳߴ�
	 *		5. CONTROL_POINT_RADIUS		�����Ƶ�İ뾶������ȷ������Ƿ��������Ƶ�
	 *		6. SCR_WIDTH				����Ļ���
	 *		7. SCR_HEIGHT				����Ļ�߶�
	 *		8. LINE_TYPE				�������û�ȴѡ����߶�����
	 *		9. RELATIVE_X_POS			����ѡ�еĿ��Ƶ���������Ծ���
	 *	   10. RELATIVE_Y_POS			����ѡ�еĿ��Ƶ���������Ծ���
	 */	
	unsigned int LINE_RESOLUTION;
	unsigned int DIMENSION;
	float LINE_WIDTH;
	bool DRAW_CONTROL_POINT;
	float  CONTROL_POINT_SIZE;
	float  CONTROL_POINT_RADIUS;
	bool VALID_SELECT;
	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;
	OrderType LINE_TYPE;
	float RELATIVE_X_POS;
	float RELATIVE_Y_POS;


	/**
	 * @ ���ã����Լ�����������Ƿ�������������ƥ��
	 * @ ������
	 *		1. data_size	�����빹�캯���е�����Ԫ�ظ���
	 *		2. type			�����������ߵĽ�������
	 */
	inline void doCheck(OrderType type) {
		unsigned int data_size = control_point_data->size();
		if (type == OrderType::FirstOrder && data_size < 4) {
			std::cout << "BAZIER::WARNING:: data_size(" << data_size << ") may not get correct result with [OrderType::FirstOrder] !\n";
		}
		else if (type == OrderType::SecondOrder && data_size < 6) {
			std::cout << "BAZIER::WARNING:: data_size(" << data_size << ") may not get correct result with [OrderType::SecondOrder] !\n";
		}
		else if (type == OrderType::ThirdOrder && data_size < 8) {
			std::cout << "BAZIER::WARNING:: data_size(" << data_size << ") may not get correct result with [OrderType::ThirdOrder] !\n";
		}
	}

	/**
	 * @ ���ã���ʼ�� OpenGL shader	 	 	
	 */
	inline void InitializeShader() {
		LineShader.InitializeProgram(LineShaderPath);
		setLineColor(1.0, 0.0, 0.0);

		ControlPointShader.InitializeProgram(ControlShaderPath);
		setPointColor(0.0, 0.0, 0.0);		
	}

public:
	// �����Ҫ���� glDrawArray ���Ƶ�����λ�ã����꣩����	
	std::vector<std::vector<float>> positions;

	/**
	 * @ ���ã��������ߵ���ɫ
	 * @ ������
	 *		1. r��red
	 *		2. g��green
	 *		3. b��blue
	 */
	inline void setLineColor(float r, float g, float b) {
		LineShader.Use();
		LineShader.setUniform3f("LineColor", r, g, b);
	}

	/**
	 * @ ���ã��������ߵĿ�ȣ���ϸ��
	 * @ ������
	 *		1. width�����ߵĿ��
	 */
	inline void setLineWidth(float width) {
		glLineWidth(width);
	}

	/**
	* @ ���ã��������ߵĿ����ģʽ����/�أ�
	* @ ������
	*		1. flag��ģʽ��/�أ�true/false��
	*/
	inline void setLineSmooth(bool flag) {
		if (flag) glEnable(GL_LINE_SMOOTH);
		else glDisable(GL_LINE_SMOOTH);
	}

	/**
	* @ ���ã����ÿ��Ƶ�Ŀ�ȣ���ϸ��
	* @ ������
	*		1. width�����Ƶ�Ĵ�С
	*/
	inline void setControlPointSize(float width) {
		glPointSize(width);
	}

	/**
	* @ ���ã����ÿ��Ƶ����ɫ
	* @ ������
	*		1. r��red
	*		2. g��green
	*		3. b��blue
	*/
	inline void setPointColor(float r, float g, float b) {
		ControlPointShader.Use();
		ControlPointShader.setUniform3f("PointColor", r, g, b);
	}

	/**
	* @ ���ã����ÿ��Ƶ�Ŀ����ģʽ����/�أ�
	* @ ������
	*		1. flag��ģʽ��/�أ�true/false��
	*/
	inline void setControlPointSmooth(bool flag) {
		if (flag) glEnable(GL_POINT_SMOOTH);
		else glDisable(GL_POINT_SMOOTH);
	}

	/**
	* @ ���ã����������ק���Ƶ㹦�ܣ�������ԭʼ���ݣ���ϸʵ��������ĵ���		
	*/
	void CursorDragControlPoint() {
		if (glfwGetMouseButton(WINDOW, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			// ��ȡ�������
			double xpos, ypos;
			glfwGetCursorPos(WINDOW, &xpos, &ypos);	
			
			// ���δѡ�п��Ƶ�״̬�µ������ѹ����
			if (!VALID_SELECT) {
				// �������Ƿ�ѡ�п��Ƶ㣬����ʵ��˼·����� document.md
				float point_size_square = CONTROL_POINT_RADIUS * CONTROL_POINT_RADIUS + 2.0f;
				for (int i = 0; i < control_point_cnt; i++) {
					float param_1 = ((((*control_point_data)[i * 2] + 1.0) / 2.0) * (float)SCR_WIDTH - xpos);
					float param_2 = (((1.0 - (*control_point_data)[i * 2 + 1]) / 2.0) * (float)SCR_HEIGHT - ypos);
					float distant_square = param_1 * param_1 + param_2 * param_2;
					if (distant_square <= point_size_square) {
						std::cout << "Point " << i << " has been selected.\n";
						current_selected_control_point_index = i;
						VALID_SELECT = true;		
						RELATIVE_X_POS = param_1;
						RELATIVE_Y_POS = param_2;
						lastXpos = xpos;
						lastYpos = xpos;
						break;
					}
				}
			}
			// ���ѡ�п��Ƶ�֮��������ѹ�������¿��Ƶ�λ�ò����»�������
			else {	
				// �߽��⣬���Ƶ㲻�ܳ������ڱ߽�
				if (xpos > SCR_WIDTH) xpos = SCR_WIDTH - CONTROL_POINT_SIZE;
				else if (xpos < 0.0f) xpos = CONTROL_POINT_SIZE;
				if (ypos > SCR_HEIGHT) ypos = SCR_HEIGHT - CONTROL_POINT_SIZE;
				else if (ypos < 0.0f) ypos = CONTROL_POINT_SIZE;

				// ���¿��Ƶ���Ϣ
				float ctr_point_x = RELATIVE_X_POS + xpos;
				float ctr_point_y = RELATIVE_Y_POS + ypos;				

				(*control_point_data)[current_selected_control_point_index * 2] = (ctr_point_x / (float)SCR_WIDTH) * 2.0f - 1.0f;
				(*control_point_data)[current_selected_control_point_index * 2 + 1] = 1.0f - (ctr_point_y / (float)SCR_HEIGHT) * 2.0f ;

				positions.clear();
				VAOs.clear();
				InitializePositions(LINE_TYPE);
				InitializeVertexArrayObject();	
				
			}
		}
		else if (glfwGetMouseButton(WINDOW, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && VALID_SELECT) {
			std::cout << "RELEASE" << std::endl;
			VALID_SELECT = false;
		}
	}

	/**
	 * @ ���ã�ʹ�����úõ� shader ��Ⱦ��������
	 */
	inline void Render() {
		LineShader.Use();
		glLineWidth(LINE_WIDTH);
		for (int i = 0; i < positions.size(); i++) {
			glBindVertexArray(VAOs[i]);			
			glDrawArrays(GL_LINE_STRIP, 0, positions[i].size() / 2);
		}
				
		if (DRAW_CONTROL_POINT) {
			ControlPointShader.Use();						
			glBindVertexArray(ControlPointVAO);
			glDrawArrays(GL_POINTS, 0, control_point_cnt);
			glLineWidth(1.0f);
			if(LINE_TYPE != OrderType::FirstOrder)
				glDrawArrays(GL_LINE_STRIP, 0, control_point_cnt);
			CursorDragControlPoint();
		}
	}

	/**
	 * @ ���ã��������ڻ������ߵĲ���
	 * @ ������
	 *		1. scr_width				����Ļ���
	 *		2. scr_height				����Ļ�߶�
	 *		3. window					����ȡ GLFW window ���󣬴Ӷ���ȡָ��λ�ã�����ʹ��ָ��������Ƶ㹦��
	 *		4. data						���û���������߿��Ƶ�����
	 *		5. data_size				���û���������߿��Ƶ����ݴ�С
	 *		6. type						�����߽�������
	 *		7. line_resolution			�����ߵľ�ϸ�̶�
	 *		8. dimension				�����ߵ�ά��
	 *		9. line_width				�����ߵĿ��
	 *	   10. draw_control_point		���Ƿ�������߿��Ƶ�
	 *	   11. control_point_size		��ȷ�����߿��Ƶ��С
	 */
	void LineConfiguration(unsigned int scr_width, unsigned int scr_height,
		GLFWwindow *window, std::vector<float>& data, OrderType type,
		float line_resolution, int dimension, int line_width, 
		bool draw_control_point, float control_point_size)
	{		

		ControlPointVAO = 0;

		control_point_data = &data;
		control_point_cnt = control_point_data->size() / 2;

		DIMENSION = dimension;
		if (type == OrderType::FirstOrder) LINE_RESOLUTION = 1;
		else if (line_resolution < 10) LINE_RESOLUTION = 10;
		else LINE_RESOLUTION = line_resolution;

		LINE_WIDTH = line_width;
		setLineWidth(LINE_WIDTH);
		setLineSmooth(true);

		DRAW_CONTROL_POINT = draw_control_point;
		CONTROL_POINT_SIZE = control_point_size;
		CONTROL_POINT_RADIUS = CONTROL_POINT_SIZE / 2.0f;
		setControlPointSize(CONTROL_POINT_SIZE);
		setControlPointSmooth(true);

		WINDOW = window;
		 
		VALID_SELECT = false;
		SCR_WIDTH = scr_width;
		SCR_HEIGHT = scr_height;

		lastXpos = 0.0f;
		lastYpos = 0.0f;

		LINE_TYPE = type;

		doCheck(type);
	}

	/**
	 * @ ���ã���ʼ�����ڻ������ߵĶ�������
	 * @ ������
	 *		1. data			���û����빹�캯����ԭʼ��������
	 *		2. data_size	����������Ԫ�ظ���
	 *		3. type			�����߽�������
	 */
	void InitializePositions(OrderType type) {
		// ��������������Ҫ�õ��Ĳ����������л���Ϊ һ�ױ��������߲��������ױ��������߲����� ���ױ��������߲���
		float T, one_minus_T;
		float T_squre, one_minus_T_squre, two_T_one_minus_T, three_T_one_minus_T_square, three_T_square_one_minus_T;
		float T_cubic, one_minus_T_cubic;

		unsigned int LENGTH = control_point_cnt - ((unsigned int)type + 2);

		if (positions.size() == 0) {
			positions.reserve(LENGTH);			
		}		

		for(int g = 0; g <= LENGTH; g++){
			std::vector<float> position;
			position.reserve((LINE_RESOLUTION + 1) * DIMENSION);
			for (int t = 0; t <= LINE_RESOLUTION; t++) {
				// ����						
				if (type == OrderType::FirstOrder || type == OrderType::SecondOrder || type == OrderType::ThirdOrder) {
					// һ�������
					T = (float)t / (float)LINE_RESOLUTION;
					one_minus_T = (1.0f - T);
				}
				if (type == OrderType::SecondOrder || type == OrderType::ThirdOrder) {
					// ���������
					T_squre = std::pow(T, 2);
					one_minus_T_squre = std::pow(one_minus_T, 2);
					two_T_one_minus_T = 2.0f * T * one_minus_T;
					three_T_one_minus_T_square = 3.0f * T * one_minus_T_squre;
					three_T_square_one_minus_T = 3.0f * T_squre * one_minus_T;
				}
				if (type == OrderType::ThirdOrder) {
					// ���������
					T_cubic = T * T_squre;
					one_minus_T_cubic = one_minus_T_squre * one_minus_T;
				}

				// ���ݱ��������߽������ͣ�ѡ���������߹�ʽ����������λ��
				// ���㷽ʽ��� document.md �е� �����������ߡ� �µ� ��������㡱
				
				for (int i = 0; i < DIMENSION; i++) {
					int i0 = i + DIMENSION * g;
					int i1 = i + DIMENSION * (g + 1);
					if (type == OrderType::FirstOrder) {						
						position.emplace_back(one_minus_T * (*control_point_data)[i0] + T * (*control_point_data)[i1]);
					}
					else if (type == OrderType::SecondOrder) {
						int i2 = i + DIMENSION * (g + 2);
						position.emplace_back(one_minus_T_squre * (*control_point_data)[i0] + two_T_one_minus_T * (*control_point_data)[i1] + T_squre * (*control_point_data)[i2]);
					}
					else if (type == OrderType::ThirdOrder) {
						int i2 = i + DIMENSION * (g + 2);
						int i3 = i + DIMENSION * (g + 3);
						position.emplace_back(one_minus_T_cubic * (*control_point_data)[i0] + three_T_one_minus_T_square * (*control_point_data)[i1] + three_T_square_one_minus_T * (*control_point_data)[i2] + T_cubic * (*control_point_data)[i3]);
					}
				}
			}
			positions.emplace_back(position);
		}		
	}

	/**
	 * @ ���ã���ʼ�� OpenGL ��Ⱦʹ�õ� Vertex Array Object
	 */
	void InitializeVertexArrayObject() {			
		for (int i = 0; i < positions.size(); i++) {
			unsigned int VAO;
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			unsigned int VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, positions[i].size() * sizeof(float), positions[i].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);
			VAOs.push_back(VAO);
		}

		if (DRAW_CONTROL_POINT) {
			if(ControlPointVAO == 0)
				glGenVertexArrays(1, &ControlPointVAO);
			glBindVertexArray(ControlPointVAO);
			unsigned int ControlPointVBO;
			glGenBuffers(1, &ControlPointVBO);
			glBindBuffer(GL_ARRAY_BUFFER, ControlPointVBO);
			glBufferData(GL_ARRAY_BUFFER, 2 * control_point_cnt * sizeof(float), control_point_data->data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);
		}
	}

	/**
	 * @ ���ã����� Bezier �����
	 * @ ������
	 *		1. scr_width				����Ļ���
	 *		2. scr_height				����Ļ�߶�
	 *		3. window					����ȡ GLFW window ���󣬴Ӷ���ȡָ��λ�ã�����ʹ��ָ��������Ƶ㹦��
	 *		4. data						���û����빹�캯����ԭʼ��������
	 *		5. data_size				�����빹�캯���е�����Ԫ�ظ���
	 *		6. type						�����߽�������
	 *		7. line_resolution			�����ߵľ�ϸ�̶�
	 *		8. dimension				�����ߵ�ά��
	 *		9. line_width				�����ߵĿ��
	 *	   10. draw_control_point		���Ƿ���ƿ��Ƶ�
	 *	   11. control_point_size		�����Ƶ��С
	 */
	Bezier(unsigned int scr_width, unsigned int scr_height, 
		GLFWwindow *window, std::vector<float>& data, OrderType type, 
		float line_resolution = 10.0, int dimension = 2, int line_width = 3, 
		bool draw_control_point = true, float control_point_size = 10.0)
	{						
		InitializeShader();
		LineConfiguration(scr_width, scr_height, window, data, type, line_resolution, dimension, line_width, draw_control_point, control_point_size);
		InitializePositions(type);
		InitializeVertexArrayObject();				
	}


};