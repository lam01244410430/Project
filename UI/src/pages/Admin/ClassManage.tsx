import React, { useState, useEffect } from 'react';
import { Table, Button, Modal, Form, Input, message, Space, Popconfirm, Tag } from 'antd';
import { PlusOutlined, EditOutlined, DeleteOutlined, TeamOutlined } from '@ant-design/icons';
import { api } from '../../services/api';

// Định nghĩa kiểu dữ liệu Class khớp với Backend C trả về
interface ClassItem {
  id: number;
  name: string;        // Tên lớp (e.g. 10A1)
  grade: string;       // Khối (e.g. 10)
  headTeacher?: string; // Tên GVCN (nếu có)
}

const ClassManage: React.FC = () => {
  const [classes, setClasses] = useState<ClassItem[]>([]);
  const [loading, setLoading] = useState(false);
  const [visible, setVisible] = useState(false);
  const [form] = Form.useForm();

  // --- 1. LOAD DỮ LIỆU TỪ API ---
  const loadClasses = async () => {
    setLoading(true);
    try {
      const res = await api.getClasses();
      // Backend trả về: { code: 200, data: [...] }
      // Map thêm key để Antd Table hoạt động đúng
      if (res && res.data) {
        const dataWithKeys = res.data.map((c: any) => ({ ...c, key: c.id }));
        setClasses(dataWithKeys);
      } else {
        setClasses([]); // Nếu data null/undefined thì set rỗng để không lỗi
      }
    } catch (error) {
      console.error("Failed to load classes", error);
      // message.error đã được xử lý ở api.ts
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadClasses();
  }, []);

  // --- 2. XỬ LÝ THÊM LỚP MỚI ---
  const handleAdd = () => {
    form.resetFields();
    setVisible(true);
  };

  const handleSubmit = async (values: any) => {
    try {
      await api.addClass(values);
      message.success('Thêm lớp học thành công!');
      setVisible(false);
      loadClasses(); // Refresh lại bảng sau khi thêm
    } catch (error) {
      // Lỗi đã được handle
    }
  };

  // --- 3. XỬ LÝ XOÁ (Optional - Nếu backend hỗ trợ) ---
  /*
  const handleDelete = async (id: number) => {
    try {
      await api.deleteClass(id); // Cần thêm API này trong api.ts và backend C
      message.success('Đã xoá lớp học');
      loadClasses();
    } catch (error) {}
  };
  */

  // --- 4. CẤU HÌNH CỘT ---
  const columns = [
    { 
      title: 'Class ID', 
      dataIndex: 'id', 
      key: 'id', 
      width: 80 
    },
    { 
      title: 'Class Name', 
      dataIndex: 'name', 
      key: 'name',
      render: (text: string) => <span style={{ fontWeight: 600, color: '#1890ff' }}>{text}</span>
    },
    { 
      title: 'Grade', 
      dataIndex: 'grade', 
      key: 'grade',
      render: (grade: string) => <Tag color="geekblue">Grade {grade}</Tag>
    },
    { 
      title: 'Head Teacher', 
      dataIndex: 'headTeacher', 
      key: 'headTeacher',
      render: (name: string) => name ? <b>{name}</b> : <span style={{ color: '#999', fontStyle: 'italic' }}>Not Assigned</span>
    },
    // Cột hành động (Sửa/Xoá) - Tạm thời ẩn nếu chưa có API
    /*
    {
      title: 'Action',
      key: 'action',
      render: (_: any, record: ClassItem) => (
        <Space>
          <Button icon={<EditOutlined />} size="small" />
          <Popconfirm title="Delete this class?" onConfirm={() => handleDelete(record.id)}>
            <Button icon={<DeleteOutlined />} size="small" danger />
          </Popconfirm>
        </Space>
      )
    }
    */
  ];

  return (
    <>
      {/* Header & Toolbar */}
      <div style={{ marginBottom: 16, display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <span style={{ fontSize: 18, fontWeight: 600 }}>Class Management</span>
        <Button type="primary" icon={<PlusOutlined />} onClick={handleAdd}>
          Add Class
        </Button>
      </div>

      {/* Data Table */}
      <Table 
        dataSource={classes} 
        columns={columns} 
        rowKey="id" 
        loading={loading} // Hiển thị xoay xoay khi đang load
        pagination={{ pageSize: 8, showTotal: (total) => `Total ${total} classes` }} 
        locale={{ emptyText: 'No classes found in database' }} // Thông báo khi không có dữ liệu
      />

      {/* Modal Add Class */}
      <Modal
        title="Create New Class"
        open={visible}
        onCancel={() => setVisible(false)}
        footer={null}
      >
        <Form form={form} onFinish={handleSubmit} layout="vertical">
          <Form.Item 
            name="name" 
            label="Class Name" 
            rules={[{ required: true, message: 'Please enter class name!' }]}
          >
            <Input placeholder="e.g., 高一A班 (Grade 10 Class A)" />
          </Form.Item>

          <Form.Item 
            name="grade" 
            label="Grade" 
            rules={[{ required: true, message: 'Please enter grade!' }]}
          >
            <Input placeholder="e.g., 10" />
          </Form.Item>

          <Button type="primary" htmlType="submit" block size="large" icon={<TeamOutlined />}>
            Save Class
          </Button>
        </Form>
      </Modal>
    </>
  );
};

export default ClassManage;