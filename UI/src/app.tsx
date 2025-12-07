import React, { useState, useEffect } from 'react';
import { Layout, Menu, theme, Button, Space, message } from 'antd';
import { 
  DashboardOutlined, 
  TeamOutlined, 
  BookOutlined, 
  UserOutlined, 
  LogoutOutlined, 
  UploadOutlined, 
  BarChartOutlined, 
  SolutionOutlined 
} from '@ant-design/icons';

// --- IMPORT PAGES ---
import Login from './pages/Login';
import Dashboard from './pages/Dashboard/index'; 
import UserManage from './pages/Admin/UserManage';
import ClassManage from './pages/Admin/ClassManage';
import SubjectManage from './pages/Admin/SubjectManage';
import ScoreEntry from './pages/Teacher/ScoreEntry';
import ClassStats from './pages/Teacher/ClassStats';
import MyScore from './pages/Student/MyScore';

const { Header, Content, Sider } = Layout;

// --- APP COMPONENT CHÍNH ---
export default function App() {
  const [user, setUser] = useState<any>(null);
  const [activeTab, setActiveTab] = useState('dashboard'); // Tab đang mở
  const [collapsed, setCollapsed] = useState(false);
  const { token: { colorBgContainer } } = theme.useToken();

  // 1. Kiểm tra đăng nhập khi mở app
  useEffect(() => {
    const storedUser = localStorage.getItem('user');
    if (storedUser) {
      setUser(JSON.parse(storedUser));
    }
  }, []);

  // 2. Hàm render nội dung dựa trên Tab đang chọn
  const renderContent = () => {
    switch (activeTab) {
      case 'dashboard': return <Dashboard />;
      case 'users': return <UserManage />;
      case 'classes': return <ClassManage />;
      case 'subjects': return <SubjectManage />;
      case 'score-entry': return <ScoreEntry />;
      case 'class-stats': return <ClassStats />;
      case 'my-scores': return <MyScore />;
      default: return <Dashboard />;
    }
  };

  // 3. Xử lý Login thành công (được gọi từ component Login)
  const handleLoginSuccess = (userData: any) => {
    localStorage.setItem('user', JSON.stringify(userData));
    setUser(userData);
    setActiveTab('dashboard');
  };

  // 4. Xử lý Logout
  const handleLogout = () => {
    localStorage.removeItem('user');
    setUser(null);
    message.success('Đã đăng xuất');
  };

  // 5. Tạo danh sách menu động theo quyền
  const getMenuItems = () => {
    const role = user?.role; 
    const items: any[] = [
      { key: 'dashboard', icon: <DashboardOutlined />, label: 'Dashboard' },
    ];

    if (role === 1) { // Admin
      items.push(
        { type: 'divider' },
        { key: 'grp_admin', label: 'Administration', type: 'group', children: [
            { key: 'users', icon: <UserOutlined />, label: 'User Management' },
            { key: 'classes', icon: <TeamOutlined />, label: 'Class Management' },
            { key: 'subjects', icon: <BookOutlined />, label: 'Subject Management' },
        ]}
      );
    }

    if (role === 2 || role === 1) { // Teacher & Admin
      items.push(
        { type: 'divider' },
        { key: 'grp_teach', label: 'Teaching', type: 'group', children: [
            { key: 'score-entry', icon: <UploadOutlined />, label: 'Score Entry' },
            { key: 'class-stats', icon: <BarChartOutlined />, label: 'Class Statistics' },
        ]}
      );
    }

    if (role === 3) { // Student
      items.push(
        { key: 'my-scores', icon: <SolutionOutlined />, label: 'My Scores' }
      );
    }
    return items;
  };

  // --- RENDERING ---

  // Nếu chưa có user -> Hiện màn hình Login
  if (!user) {
    // Chúng ta cần sửa nhẹ file Login.tsx để nhận props onLoginSuccess
    // Nhưng để nhanh, ta có thể giả lập một component wrapper ở đây hoặc sửa Login sau.
    // Ở đây tôi sẽ Render Login và truyền một prop giả (nếu Login của bạn hỗ trợ)
    // Hoặc đơn giản là render Login và để nó tự handle localStorage, 
    // sau đó ta reload trang (cách cũ của bạn). 
    
    // Cách tốt nhất: Sửa Login để nhận callback. Nhưng để tương thích code cũ:
    return <Login />; 
    // Lưu ý: Code Login cũ của bạn dùng window.location.href = '/dashboard', 
    // điều này sẽ reload lại trang và chạy vào useEffect ở trên -> set user -> vào Layout.
    // Nên cách này VẪN HOẠT ĐỘNG tốt với code cũ.
  }

  // Nếu đã có user -> Hiện Layout chính
  return (
    <Layout style={{ minHeight: '100vh' }}>
      <Sider 
        collapsible 
        collapsed={collapsed} 
        onCollapse={(value) => setCollapsed(value)}
        width={260}
        style={{ 
          overflow: 'auto', height: '100vh', position: 'fixed', left: 0, top: 0, bottom: 0, zIndex: 100 
        }}
      >
        <div style={{ 
          height: 64, margin: 16, background: 'rgba(255,255,255,0.2)', borderRadius: 6, 
          display: 'flex', alignItems: 'center', justifyContent: 'center', 
          color: 'white', fontWeight: 'bold', fontSize: collapsed ? 14 : 18,
          overflow: 'hidden', whiteSpace: 'nowrap'
        }}>
          {collapsed ? 'SC' : 'SmartCampus'}
        </div>
        
        <Menu
          theme="dark"
          mode="inline"
          selectedKeys={[activeTab]} // Highlight tab đang chọn
          onClick={(e) => setActiveTab(e.key)} // Đổi tab khi click
          items={getMenuItems()}
        />
      </Sider>

      <Layout style={{ marginLeft: collapsed ? 80 : 260, transition: 'all 0.2s' }}>
        <Header style={{ 
          padding: '0 24px', background: colorBgContainer, position: 'sticky', top: 0, zIndex: 99, width: '100%',
          display: 'flex', justifyContent: 'space-between', alignItems: 'center',
          boxShadow: '0 1px 4px rgba(0,21,41,0.08)'
        }}>
          <div style={{ fontSize: 18, fontWeight: 600, color: '#001529' }}>
             Student Score Management System
          </div>
          <Space>
            <div style={{ textAlign: 'right', lineHeight: '1.2' }}>
              <div style={{ fontWeight: 'bold' }}>{user?.realName}</div>
              <div style={{ fontSize: 12, color: '#888' }}>{user?.username}</div>
            </div>
            <Button type="primary" danger icon={<LogoutOutlined />} onClick={handleLogout} size="small">
              Logout
            </Button>
          </Space>
        </Header>

        <Content style={{ margin: '24px 16px 0', overflow: 'initial' }}>
          <div style={{ padding: 24, minHeight: '85vh', background: colorBgContainer, borderRadius: 8 }}>
            {/* Render nội dung động dựa trên state activeTab */}
            {renderContent()}
          </div>
        </Content>
      </Layout>
    </Layout>
  );
}