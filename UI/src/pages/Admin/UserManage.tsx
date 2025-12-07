import React, { useState, useEffect } from 'react';
import { Table, Button, Modal, Form, Input, Select, message, Tag, Popconfirm, Space } from 'antd';
import { PlusOutlined, EditOutlined, DeleteOutlined, UserAddOutlined } from '@ant-design/icons';
import { api } from '../../services/api';

const { Option } = Select;

// Interface khớp với dữ liệu trả về từ Backend C
interface User {
  id: number;          // Backend C dùng 'id' hoặc '用户ID'
  username: string;    // '用户名'
  realName: string;    // '真实姓名'
  role: number;        // 1=Admin, 2=Teacher, 3=Student
  className?: string;  // Class name (nếu có)
  status?: number;     // 1=Active, 0=Inactive
}

const UserManage: React.FC = () => {
  const [users, setUsers] = useState<User[]>([]);
  const [loading, setLoading] = useState(false);
  const [visible, setVisible] = useState(false);
  const [editingUser, setEditingUser] = useState<User | null>(null);
  const [form] = Form.useForm();

  // --- 1. LOAD DATA TỪ DATABASE ---
  const loadUsers = async () => {
    setLoading(true);
    try {
      const res = await api.getUsers(); 
      // Backend trả về: { code: 200, data: [...] }
      // Map thêm key để Antd Table không báo lỗi
      const data = Array.isArray(res.data) ? res.data.map((u: any) => ({ ...u, key: u.id })) : [];
      setUsers(data);
    } catch (error) {
      console.error("Failed to load users", error);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadUsers();
  }, []);

  // --- 2. XỬ LÝ THÊM / SỬA ---
  const handleAdd = () => {
    setEditingUser(null);
    form.resetFields();
    setVisible(true);
  };

  const handleEdit = (record: User) => {
    setEditingUser(record);
    form.setFieldsValue({
      username: record.username,
      realName: record.realName,
      role: record.role,
      // Mật khẩu không hiển thị khi edit để bảo mật
    });
    setVisible(true);
  };

  const handleSubmit = async (values: any) => {
    try {
      if (editingUser) {
        // Update logic (Nếu backend hỗ trợ update)
        // await api.updateUser({ ...values, id: editingUser.id });
        message.warning("Tính năng cập nhật đang phát triển"); 
      } else {
        // Create new user
        await api.addUser(values);
        message.success('User created successfully');
      }
      setVisible(false);
      loadUsers(); // Refresh lại bảng từ DB
    } catch (error) {
      // Lỗi đã được xử lý ở api.ts
    }
  };

  // --- 3. XỬ LÝ XOÁ ---
  const handleDelete = async (id: number) => {
    try {
      await api.deleteUser(id);
      message.success('User deleted successfully');
      loadUsers(); // Refresh lại bảng từ DB
    } catch (error) {
      // Error handled
    }
  };

  // --- 4. CẤU HÌNH CỘT BẢNG ---
  const columns = [
    { 
      title: 'ID', 
      dataIndex: 'id', 
      key: 'id', 
      width: 70 
    },
    { 
      title: 'Username', 
      dataIndex: 'username', 
      key: 'username',
      render: (text: string) => <b>{text}</b>
    },
    { 
      title: 'Real Name', 
      dataIndex: 'realName', 
      key: 'realName' 
    },
    { 
      title: 'Role', 
      dataIndex: 'role', 
      key: 'role',
      render: (role: number) => {
        if (role === 1) return <Tag color="red">Administrator</Tag>;
        if (role === 2) return <Tag color="blue">Teacher</Tag>;
        return <Tag color="green">Student</Tag>;
      }
    },
    {
      title: 'Action',
      key: 'action',
      render: (_: any, record: User) => (
        <Space>
          <Button 
            icon={<EditOutlined />} 
            size="small" 
            onClick={() => handleEdit(record)} 
          />
          <Popconfirm 
            title="Delete this user?"
            onConfirm={() => handleDelete(record.id)}
            okText="Yes"
            cancelText="No"
          >
            <Button icon={<DeleteOutlined />} size="small" danger />
          </Popconfirm>
        </Space>
      ),
    },
  ];

  return (
    <>
      {/* TOOLBAR */}
      <div style={{ marginBottom: 16, display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <span style={{ fontSize: 18, fontWeight: 600 }}>User Management</span>
        <Button type="primary" icon={<UserAddOutlined />} onClick={handleAdd}>
          Add User
        </Button>
      </div>

      {/* DATA TABLE */}
      <Table 
        dataSource={users} 
        columns={columns} 
        rowKey="id" 
        loading={loading}
        pagination={{ pageSize: 10, showTotal: (total) => `Total ${total} users` }} 
      />

      {/* MODAL FORM */}
      <Modal
        title={editingUser ? "Edit User" : "Create New User"}
        open={visible}
        onCancel={() => setVisible(false)}
        footer={null}
      >
        <Form form={form} onFinish={handleSubmit} layout="vertical">
          <Form.Item 
            name="username" 
            label="Username" 
            rules={[{ required: true, message: 'Required' }]}
          >
            <Input placeholder="Username" disabled={!!editingUser} />
          </Form.Item>

          {!editingUser && (
            <Form.Item 
              name="password" 
              label="Password" 
              rules={[{ required: true, message: 'Required' }]}
            >
              <Input.Password placeholder="Password" />
            </Form.Item>
          )}

          <Form.Item 
            name="realName" 
            label="Real Name" 
            rules={[{ required: true, message: 'Required' }]}
          >
            <Input placeholder="Full Name (Chinese)" />
          </Form.Item>

          <Form.Item name="role" label="Role" initialValue={3}>
            <Select>
              <Option value={1}>Administrator</Option>
              <Option value={2}>Teacher</Option>
              <Option value={3}>Student</Option>
            </Select>
          </Form.Item>

          <Button type="primary" htmlType="submit" block>
            {editingUser ? 'Update' : 'Create'}
          </Button>
        </Form>
      </Modal>
    </>
  );
};

export default UserManage;