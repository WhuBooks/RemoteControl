

#include <string>


using std::string;

class CHTTP_post {
public:
	CHTTP_post() = default;
	~CHTTP_post() = default;

	/**
	 * @brief 发送普通 post json 请求
	 * @param url   POST URL
	 * @param post  POST 内容字符串
	 * @param result POST 返回结果字符串
	 * @param errmsg 如果出错，写入错误信息
	 * @return 成功返回 true， 失败返回 false
	 */
	bool xfuture_http_post_json(string url, string post, string &result, string &errmsg);


	/**
	 * @brief 发送 pgm post 请求
	 * @param url  POST URL
	 * @param pgm  POST PGM 内容
	 * @param result POST 返回结果字符串
	 * @param errmsg  如果出错，写入错误信息
	 * @return  成功返回 true, 失败返回 false
	 */
	bool xfuture_http_post_pgm(string url, string pgm, string &result, string &errmsg);

	/**
	 * @brief 发送 post 请求
	 * @param url   POST URL
	 * @param raw   POST 内容
	 * @param result POST 返回结果字符串
	 * @param errmsg    如果出错，写入错误信息
	 * @return  成功返回 true, 失败返回 false
	 */
	bool xfuture_http_post_raw(string url, string raw, string &result, string &errmsg);

	/**
	 * @brief 对文件进行 base64 编码
	 * @param file 文件路径，不支持超过 1MB 大小的文件
	 * @param out 编码后的文件内容
	 * @return   返回编码后的文件内容字节数，返回 -1 表示出错
	 */
	int xfuture_http_base64_encode_file(string file, string &out);

};
