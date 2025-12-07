import React, { useState } from 'react';
import { Form, Input, Button, Card, message } from 'antd';
import { UserOutlined, LockOutlined } from '@ant-design/icons';
import { api } from '../services/api'; // Dùng api thật

// Định nghĩa Props
interface LoginProps {
  onLoginSuccess?: (user: any) => void;
}

const Login: React.FC<LoginProps> = ({ onLoginSuccess }) => {
  const [loading, setLoading] = useState(false);

  const onFinish = async (values: any) => {
    setLoading(true);
    try {
      // Gọi API thật từ Backend C
      const res = await api.login(values);
      
      if (res && res.code === 200) {
        message.success('Login successful!');
        
        // Lấy thông tin user từ response
        const userData = res.data || { username: values.username, role: 3 }; // Fallback nếu backend chưa trả đủ
        
        // Gọi hàm callback để App cập nhật state
        if (onLoginSuccess) {
          onLoginSuccess(userData);
        } else {
          // Fallback cho code cũ (reload trang)
          localStorage.setItem('user', JSON.stringify(userData));
          window.location.href = '/';
        }
      } else {
        message.error(res.msg || 'Login failed');
      }
    } catch (error) {
      message.error('Connection error');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div style={{ height: '100vh', display: 'flex', justifyContent: 'center', alignItems: 'center', background: '#f0f2f5' }}>
      <Card title="SmartCampus Login" style={{ width: 400, boxShadow: '0 4px 12px rgba(0,0,0,0.1)' }}>
        <Form onFinish={onFinish} size="large">
          <Form.Item name="username" rules={[{ required: true, message: 'Please enter username' }]}>
            <Input prefix={<UserOutlined />} placeholder="Username" />
          </Form.Item>
          <Form.Item name="password" rules={[{ required: true, message: 'Please enter password' }]}>
            <Input.Password prefix={<LockOutlined />} placeholder="Password" />
          </Form.Item>
          <Button type="primary" htmlType="submit" loading={loading} block>
            Login
          </Button>
        </Form>
      </Card>
    </div>
  );
};

export default Login;