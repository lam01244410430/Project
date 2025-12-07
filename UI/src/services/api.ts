import { message } from 'antd';

const API_BASE = 'http://localhost:8080/api';

/**
 * Hàm wrapper gọi API an toàn hơn.
 * Tự động xử lý trường hợp response rỗng hoặc lỗi mạng.
 */
const request = async (endpoint: string, options?: RequestInit) => {
  try {
    const url = `${API_BASE}${endpoint}`;
    const headers = {
      'Content-Type': 'application/json',
      ...options?.headers,
    };

    const response = await fetch(url, { ...options, headers });

    // 1. Kiểm tra nếu response body rỗng (Content-Length = 0)
    const contentLength = response.headers.get("Content-Length");
    if (contentLength === "0") {
      if (!response.ok) throw new Error(`Server Error: ${response.status}`);
      return {}; // Trả về object rỗng nếu không có body
    }

    // 2. Đọc text trước để kiểm tra (An toàn hơn gọi thẳng .json())
    const text = await response.text();
    let data;
    try {
      // Chỉ parse nếu chuỗi không rỗng
      data = text ? JSON.parse(text) : {};
    } catch (e) {
      console.warn(`API ${endpoint} trả về dữ liệu không phải JSON:`, text);
      // Nếu không phải JSON (ví dụ lỗi 404/500 HTML), coi như lỗi
      if (!response.ok) throw new Error(text || `Server Error: ${response.status}`);
      return { message: text };
    }

    // 3. Nếu HTTP Status không phải 2xx -> Ném lỗi để component bắt (catch)
    if (!response.ok) {
      throw new Error(data.msg || data.error || `Lỗi máy chủ (${response.status})`);
    }

    return data;

  } catch (error: any) {
    console.error("API Request Failed:", error);
    // Ném lỗi để component gọi (UserManage, etc.) có thể xử lý
    throw error;
  }
};

export const api = {
  // --- AUTH ---
  login: (data: any) => request('/login', { 
    method: 'POST', 
    body: JSON.stringify(data) 
  }),

  // --- DASHBOARD & STATS ---
  getStats: () => request('/stats/subject-summary'),

  // --- CLASS ---
  getClasses: () => request('/classes'),
  addClass: (data: any) => request('/classes/add', { 
    method: 'POST', 
    body: JSON.stringify(data) 
  }),

  // --- SUBJECT ---
  getSubjects: () => request('/subjects'),
  addSubject: (data: any) => request('/subjects/add', { 
    method: 'POST', 
    body: JSON.stringify(data) 
  }),

  // --- USERS MANAGEMENT ---
  // Đã cập nhật đúng endpoint lấy toàn bộ user
  getUsers: () => request('/users'), 
  
  addUser: (data: any) => request('/users/add', { 
    method: 'POST', 
    body: JSON.stringify(data) 
  }),
  
  deleteUser: (id: number) => request('/users/delete', { 
    method: 'POST', 
    body: JSON.stringify({ id }) 
  }),

  // --- EXAM & TERM ---
  getTerms: () => request('/terms'),
  getExams: () => request('/exams'),

  // --- SCORES ---
  batchImport: (data: any[]) => request('/scores/batch', { 
    method: 'POST', 
    body: JSON.stringify(data) 
  }),
  
  getMyScores: (studentId: number) => request(`/scores/student?id=${studentId}`),
};