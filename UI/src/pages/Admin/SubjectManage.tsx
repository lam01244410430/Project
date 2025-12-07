import React, { useState, useEffect } from 'react';
import { Table, Button, Modal, Form, InputNumber, Input, message, Tag, Space, Popconfirm } from 'antd';
import { PlusOutlined, BookOutlined, EditOutlined, DeleteOutlined } from '@ant-design/icons';
import { api } from '../../services/api';

// Định nghĩa kiểu dữ liệu Subject khớp với Backend C
interface SubjectItem {
  id: number;
  name: string;       // Tên môn học (e.g. Mathematics)
  fullScore: number;  // Điểm tối đa (e.g. 100)
}

const SubjectManage: React.FC = () => {
  const [subjects, setSubjects] = useState<SubjectItem[]>([]);
  const [loading, setLoading] = useState(false);
  const [visible, setVisible] = useState(false);
  const [form] = Form.useForm();

  // --- 1. LOAD DỮ LIỆU TỪ API ---
  const loadSubjects = async () => {
    setLoading(true);
    try {
      const res = await api.getSubjects();
      // Backend trả về: { code: 200, data: [...] }
      if (res && res.data) {
        const dataWithKeys = res.data.map((s: any) => ({ ...s, key: s.id }));
        setSubjects(dataWithKeys);
      } else {
        setSubjects([]);
      }
    } catch (error) {
      console.error("Failed to load subjects", error);
      // Error handled in api.ts
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadSubjects();
  }, []);

  // --- 2. XỬ LÝ THÊM MÔN HỌC ---
  const handleAdd = () => {
    form.resetFields();
    setVisible(true);
  };

  const handleSubmit = async (values: any) => {
    try {
      await api.addSubject(values);
      message.success('Subject added successfully!');
      setVisible(false);
      loadSubjects(); // Refresh bảng sau khi thêm
    } catch (error) {
      // Error handled
    }
  };

  // --- 3. XỬ LÝ XOÁ (Optional) ---
  /*
  const handleDelete = async (id: number) => {
    try {
      // Cần thêm api.deleteSubject(id) nếu backend hỗ trợ
      message.info("Delete feature coming soon");
    } catch (error) {}
  };
  */

  // --- 4. CẤU HÌNH CỘT ---
  const columns = [
    { 
      title: 'ID', 
      dataIndex: 'id', 
      key: 'id', 
      width: 80 
    },
    { 
      title: 'Subject Name', 
      dataIndex: 'name', 
      key: 'name',
      render: (text: string) => <span style={{ fontWeight: 600, fontSize: '15px' }}>{text}</span>
    },
    { 
      title: 'Full Score', 
      dataIndex: 'fullScore', 
      key: 'fullScore',
      render: (score: number) => <Tag color="green">{score} pts</Tag>
    },
    // Cột hành động (Ẩn nếu chưa có API Xoá/Sửa)
    /*
    {
      title: 'Action',
      key: 'action',
      render: (_: any, record: SubjectItem) => (
        <Space>
          <Button icon={<EditOutlined />} size="small" />
          <Popconfirm title="Delete?" onConfirm={() => handleDelete(record.id)}>
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
        <span style={{ fontSize: 18, fontWeight: 600 }}>Subject Management</span>
        <Button type="primary" icon={<PlusOutlined />} onClick={handleAdd}>
          Add Subject
        </Button>
      </div>

      {/* Data Table */}
      <Table 
        dataSource={subjects} 
        columns={columns} 
        rowKey="id" 
        loading={loading} // Hiệu ứng loading
        pagination={{ pageSize: 10 }} 
        locale={{ emptyText: 'No subjects found in database' }}
      />

      {/* Modal Add Subject */}
      <Modal 
        title="Add New Subject" 
        open={visible} 
        onCancel={() => setVisible(false)} 
        footer={null}
      >
        <Form form={form} onFinish={handleSubmit} layout="vertical">
          <Form.Item 
            name="name" 
            label="Subject Name" 
            rules={[{ required: true, message: 'Please enter subject name' }]}
          >
            <Input placeholder="e.g., Mathematics (数学)" prefix={<BookOutlined />} />
          </Form.Item>

          <Form.Item 
            name="fullScore" 
            label="Full Score" 
            initialValue={100}
            rules={[{ required: true, message: 'Please enter full score' }]}
          >
            <InputNumber min={10} max={200} style={{ width: '100%' }} />
          </Form.Item>

          <Button type="primary" htmlType="submit" block size="large">
            Save Subject
          </Button>
        </Form>
      </Modal>
    </>
  );
};

export default SubjectManage;