#pragma once
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class Bezier {
public:
	// 确定贝塞尔曲线的阶数（一阶，二阶，三阶）
	static enum class OrderType {
		FirstOrder = 0, SecondOrder = 1, ThirdOrder = 2
	};
private:
	// 用于 OpenGL 渲染的 Vertex Array Object
	std::vector<unsigned int> VAOs;
	unsigned int ControlPointVAO;
	
	// 用于 OpenGL 渲染的 shader 路径
	const char* LineShaderPath = "src/shader/Line.shader";	
	const char* ControlShaderPath = "src/shader/ControlPoint.shader";

	// 存储用户传入构造函数的各个节点数据，用于绘制控制贝塞尔曲线绘制的节点
	std::vector<float>* control_point_data;
	unsigned int control_point_cnt;

	// 获取 GLFW window 对象，从而获取指针位置，用于使用指针调整控制点功能
	GLFWwindow *WINDOW;

	// OpenGL shader
	Shader LineShader;
	Shader ControlPointShader;

	// 记录光标上一次的坐标位置
	float lastXpos;
	float lastYpos;
	unsigned int current_selected_control_point_index;
	

	/**
	 * 用于构建贝塞尔曲线的参数
	 *		1. LINE_RESOLUTION			：确定曲线由多少条直线绘制而成（确定曲线的精细程度），最低 10 条，默认为 10 条
	 *		2. DIMENSION				：确定需要构建的曲线维度，目前仅支持 2 维
	 *		3. LINE_WIDTH				：确定曲线的宽度，默认为 3
	 *		4. DRAW_CONTROL_POINT		: 确定是否绘制控制点
	 *		5. CONTROL_POINT_SIZE		：确定控制点的尺寸
	 *		5. CONTROL_POINT_RADIUS		：控制点的半径，用于确定光标是否点击到控制点
	 *		6. SCR_WIDTH				：屏幕宽度
	 *		7. SCR_HEIGHT				：屏幕高度
	 *		8. LINE_TYPE				：保留用户却选择的线段类型
	 *		9. RELATIVE_X_POS			：被选中的控制点与鼠标的相对距离
	 *	   10. RELATIVE_Y_POS			：被选中的控制点与鼠标的相对距离
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
	 * @ 作用：粗略检查输入数据是否与曲线类型相匹配
	 * @ 参数：
	 *		1. data_size	：传入构造函数中的数组元素个数
	 *		2. type			：贝塞尔曲线的阶数类型
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
	 * @ 作用：初始化 OpenGL shader	 	 	
	 */
	inline void InitializeShader() {
		LineShader.InitializeProgram(LineShaderPath);
		setLineColor(1.0, 0.0, 0.0);

		ControlPointShader.InitializeProgram(ControlShaderPath);
		setPointColor(0.0, 0.0, 0.0);		
	}

public:
	// 存放需要交给 glDrawArray 绘制的所有位置（坐标）数据	
	std::vector<std::vector<float>> positions;

	/**
	 * @ 作用：设置曲线的颜色
	 * @ 参数：
	 *		1. r：red
	 *		2. g：green
	 *		3. b：blue
	 */
	inline void setLineColor(float r, float g, float b) {
		LineShader.Use();
		LineShader.setUniform3f("LineColor", r, g, b);
	}

	/**
	 * @ 作用：设置曲线的宽度（粗细）
	 * @ 参数：
	 *		1. width：曲线的宽度
	 */
	inline void setLineWidth(float width) {
		glLineWidth(width);
	}

	/**
	* @ 作用：设置曲线的抗锯齿模式（开/关）
	* @ 参数：
	*		1. flag：模式开/关（true/false）
	*/
	inline void setLineSmooth(bool flag) {
		if (flag) glEnable(GL_LINE_SMOOTH);
		else glDisable(GL_LINE_SMOOTH);
	}

	/**
	* @ 作用：设置控制点的宽度（粗细）
	* @ 参数：
	*		1. width：控制点的大小
	*/
	inline void setControlPointSize(float width) {
		glPointSize(width);
	}

	/**
	* @ 作用：设置控制点的颜色
	* @ 参数：
	*		1. r：red
	*		2. g：green
	*		3. b：blue
	*/
	inline void setPointColor(float r, float g, float b) {
		ControlPointShader.Use();
		ControlPointShader.setUniform3f("PointColor", r, g, b);
	}

	/**
	* @ 作用：设置控制点的抗锯齿模式（开/关）
	* @ 参数：
	*		1. flag：模式开/关（true/false）
	*/
	inline void setControlPointSmooth(bool flag) {
		if (flag) glEnable(GL_POINT_SMOOTH);
		else glDisable(GL_POINT_SMOOTH);
	}

	/**
	* @ 作用：配置鼠标拖拽控制点功能，并更新原始数据（详细实现请参阅文档）		
	*/
	void CursorDragControlPoint() {
		if (glfwGetMouseButton(WINDOW, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			// 获取鼠标坐标
			double xpos, ypos;
			glfwGetCursorPos(WINDOW, &xpos, &ypos);	
			
			// 鼠标未选中控制点状态下的左键按压处理
			if (!VALID_SELECT) {
				// 检测鼠标是否选中控制点，具体实现思路请参阅 document.md
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
			// 鼠标选中控制点之后的左键按压处理，更新控制点位置并重新绘制曲线
			else {	
				// 边界检测，控制点不能超出窗口边界
				if (xpos > SCR_WIDTH) xpos = SCR_WIDTH - CONTROL_POINT_SIZE;
				else if (xpos < 0.0f) xpos = CONTROL_POINT_SIZE;
				if (ypos > SCR_HEIGHT) ypos = SCR_HEIGHT - CONTROL_POINT_SIZE;
				else if (ypos < 0.0f) ypos = CONTROL_POINT_SIZE;

				// 更新控制点信息
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
	 * @ 作用：使用配置好的 shader 渲染顶点数据
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
	 * @ 作用：配置用于绘制曲线的参数
	 * @ 参数：
	 *		1. scr_width				：屏幕宽度
	 *		2. scr_height				：屏幕高度
	 *		3. window					：获取 GLFW window 对象，从而获取指针位置，用于使用指针调整控制点功能
	 *		4. data						：用户传入的曲线控制点数据
	 *		5. data_size				：用户传入的曲线控制点数据大小
	 *		6. type						：曲线阶数类型
	 *		7. line_resolution			：曲线的精细程度
	 *		8. dimension				：曲线的维度
	 *		9. line_width				：曲线的宽度
	 *	   10. draw_control_point		：是否绘制曲线控制点
	 *	   11. control_point_size		：确定曲线控制点大小
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
	 * @ 作用：初始化用于绘制曲线的顶点坐标
	 * @ 参数：
	 *		1. data			：用户传入构造函数的原始顶点数据
	 *		2. data_size	：顶点数据元素个数
	 *		3. type			：曲线阶数类型
	 */
	void InitializePositions(OrderType type) {
		// 贝塞尔曲线中需要用到的参数，按照行划分为 一阶贝塞尔曲线参数，二阶贝塞尔曲线参数， 三阶贝塞尔曲线参数
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
				// 参数						
				if (type == OrderType::FirstOrder || type == OrderType::SecondOrder || type == OrderType::ThirdOrder) {
					// 一次项参数
					T = (float)t / (float)LINE_RESOLUTION;
					one_minus_T = (1.0f - T);
				}
				if (type == OrderType::SecondOrder || type == OrderType::ThirdOrder) {
					// 二次项参数
					T_squre = std::pow(T, 2);
					one_minus_T_squre = std::pow(one_minus_T, 2);
					two_T_one_minus_T = 2.0f * T * one_minus_T;
					three_T_one_minus_T_square = 3.0f * T * one_minus_T_squre;
					three_T_square_one_minus_T = 3.0f * T_squre * one_minus_T;
				}
				if (type == OrderType::ThirdOrder) {
					// 三次项参数
					T_cubic = T * T_squre;
					one_minus_T_cubic = one_minus_T_squre * one_minus_T;
				}

				// 根据贝塞尔曲线阶数类型，选择贝塞尔曲线公式，计算坐标位置
				// 计算方式详见 document.md 中的 “贝塞尔曲线” 下的 “坐标计算”
				
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
	 * @ 作用：初始化 OpenGL 渲染使用的 Vertex Array Object
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
	 * @ 作用：构建 Bezier 类对象
	 * @ 参数：
	 *		1. scr_width				：屏幕宽度
	 *		2. scr_height				：屏幕高度
	 *		3. window					：获取 GLFW window 对象，从而获取指针位置，用于使用指针调整控制点功能
	 *		4. data						：用户传入构造函数的原始顶点数据
	 *		5. data_size				：传入构造函数中的数组元素个数
	 *		6. type						：曲线阶数类型
	 *		7. line_resolution			：曲线的精细程度
	 *		8. dimension				：曲线的维度
	 *		9. line_width				：曲线的宽度
	 *	   10. draw_control_point		：是否绘制控制点
	 *	   11. control_point_size		：控制点大小
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