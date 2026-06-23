# 🏗️ Smart Nose TPA - Architecture & Data Flow

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        Smart Nose TPA v2.0                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌──────────────────────────────────────────────────────────────────┐       │
│  │                        VERCEL HOSTING                            │       │
│  │                    (smartnose-xxx.vercel.app)                    │       │
│  │                                                                  │       │
│  │  ┌─────────────────────────────────────────────────────────┐    │       │
│  │  │  Frontend Application                                  │    │       │
│  │  │  ├── Dashboard (Real-time Monitoring)                 │    │       │
│  │  │  ├── Map Page (GPS Locations)                         │    │       │
│  │  │  ├── History (Charts & Data)                          │    │       │
│  │  │  ├── Alerts (Notifikasi & Log)                        │    │       │
│  │  │  └── Settings (Configuration)                         │    │       │
│  │  └─────────────────────────────────────────────────────────┘    │       │
│  │                            │                                    │       │
│  │                            ▼                                    │       │
│  │  ┌─────────────────────────────────────────────────────────┐    │       │
│  │  │  Serverless API (Node.js)                             │    │       │
│  │  │  ├── POST /api/readings      (Accept sensor data)     │    │       │
│  │  │  ├── Validation              (Check thresholds)       │    │       │
│  │  │  └── Database Insert         (Save to Supabase)       │    │       │
│  │  └─────────────────────────────────────────────────────────┘    │       │
│  └──────────────────────────────────────────────────────────────────┘       │
│                            │                                                │
│                            ▼                                                │
│  ┌─────────────────────────────────────────────────────────┐                │
│  │              SUPABASE (PostgreSQL Backend)              │                │
│  │        (yhgmhmfrnpeoaihmvpvk.supabase.co)               │                │
│  │                                                         │                │
│  │  ┌──────────────────────────────────────────────────┐   │                │
│  │  │ Database Tables                                │   │                │
│  │  │                                                │   │                │
│  │  │  • sensor_readings                            │   │                │
│  │  │    ├── device_id (TPA_01, TPA_02, ...)       │   │                │
│  │  │    ├── ch4_ppm, h2s_ppm                      │   │                │
│  │  │    ├── temperature, humidity                 │   │                │
│  │  │    ├── latitude, longitude (GPS)             │   │                │
│  │  │    ├── alert_ch4, alert_h2s (flags)          │   │                │
│  │  │    └── created_at (timestamp)                │   │                │
│  │  │                                                │   │                │
│  │  │  • alerts_log                                 │   │                │
│  │  │    ├── device_id                              │   │                │
│  │  │    ├── alert_type (CH4, H2S)                 │   │                │
│  │  │    ├── severity (CRITICAL, WARNING)          │   │                │
│  │  │    ├── ppm_value                             │   │                │
│  │  │    └── created_at (timestamp)                │   │                │
│  │  └──────────────────────────────────────────────────┘   │                │
│  │                                                         │                │
│  │  ✨ Features:                                           │                │
│  │  • Realtime subscriptions (WebSocket)                 │                │
│  │  • Row Level Security (RLS)                           │                │
│  │  • Automatic backups                                  │                │
│  │  └─────────────────────────────────────────────────────│                │
│                                                                              │
│  ┌──────────────────────────────────────────────────────┐                   │
│  │           ESP32 Sensor Nodes (IoT Edge)             │                   │
│  │                                                      │                   │
│  │  TPA_01: CH4 + H2S + DHT22 + GPS                   │                   │
│  │  TPA_02: CH4 + H2S + DHT22 + GPS                   │                   │
│  │  TPA_03: CH4 + H2S + DHT22 + GPS                   │                   │
│  │  ...                                                │                   │
│  └──────────────────────────────────────────────────────┘                   │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Data Flow Diagram

### 1️⃣ Sensor → Vercel API → Supabase

```
ESP32 Sensor Node
      │
      │ HTTP POST (JSON payload)
      │ Every 60 seconds
      ▼
Vercel API (/api/readings)
      │
      ├─ Validate JSON payload
      ├─ Parse sensor data
      ├─ Check alert thresholds
      │
      ▼
Supabase PostgreSQL
      │
      ├─ Insert into sensor_readings
      ├─ Insert into alerts_log (if threshold exceeded)
      │
      ▼
Realtime Webhook
      │
      ▼
Frontend Application (Auto-refresh)
```

### 2️⃣ Frontend → Supabase → Display

```
Browser/PWA
      │
      ├─ Query latest sensor_readings
      ├─ Query alerts_log
      ├─ Subscribe to realtime updates (WebSocket)
      │
      ▼
Supabase Client
      │
      ▼
PostgreSQL Database
      │
      ▼
Frontend Display
      │
      ├─ Dashboard (Real-time gauge)
      ├─ Chart.js (Historical data)
      ├─ Alert Banner (Critical status)
      └─ Map (GPS locations)
```

---

## Deployment Architecture

```
GitHub Repository
      │ (git push)
      ▼
GitHub Webhooks
      │
      ▼
Vercel Build Pipeline
      │
      ├─ Detect framework (Static Site)
      ├─ Build (skip - no build needed)
      ├─ Optimize assets
      ├─ Deploy to Vercel CDN
      │
      ▼
Vercel Edge Network (Global CDN)
      │
      ├─ California, USA
      ├─ Frankfurt, Germany
      ├─ Singapore, Asia
      └─ Other regions
      │
      ▼
Your Custom Domain (Optional)
      │
      ▼
End Users (Browser)
```

---

## Technology Stack

```
Frontend:
├── HTML5 / CSS3 / Vanilla JavaScript
├── Chart.js (Data visualization)
├── Supabase.js Client Library
└── Responsive Design (Mobile-first)

Backend:
├── Vercel Functions (Serverless Node.js)
├── express-like routing
└── @supabase/supabase-js client

Database:
├── PostgreSQL (Supabase)
├── Row Level Security (RLS)
├── Realtime subscriptions
└── Automatic backups

Infrastructure:
├── Vercel (Hosting & API)
├── Supabase (Database & Auth)
├── GitHub (Source Control)
└── CloudFlare (Optional CDN)

IoT/Edge:
├── ESP32 microcontroller
├── Arduino framework
├── WiFi connectivity
└── Sensor modules (analog/I2C)
```

---

## API Endpoint Specification

### POST /api/readings

**Request:**
```json
{
  "device_id": "TPA_01",
  "ch4_ppm": 200.5,
  "h2s_ppm": 10.2,
  "temperature": 28.5,
  "humidity": 75.0,
  "latitude": -7.6298,
  "longitude": 111.5098,
  "gps_valid": true
}
```

**Response (Success):**
```json
{
  "success": true,
  "message": "Data recorded successfully",
  "id": 12345
}
```

**Response (Error):**
```json
{
  "error": "Missing required fields",
  "details": "..."
}
```

**Status Codes:**
- `200` - Success
- `400` - Bad request (missing fields)
- `405` - Method not allowed
- `500` - Server error

---

## Security Architecture

```
┌─────────────────────────────────────────┐
│      Public Internet (HTTPS)            │
└──────────────┬──────────────────────────┘
               │
               ▼
    ┌──────────────────────┐
    │  Vercel Edge (CORS)  │
    └──────────┬───────────┘
               │
               ▼
    ┌──────────────────────┐
    │  Vercel Functions    │
    │  ├─ Validate input   │
    │  ├─ Check JWT token  │
    │  └─ Rate limiting    │
    └──────────┬───────────┘
               │
               ▼
    ┌──────────────────────┐
    │  Supabase (RLS)      │
    │  ├─ Row Level Security
    │  ├─ Service Role Key │
    │  ├─ Anon Key (R/O)   │
    │  └─ Encryption       │
    └──────────────────────┘
```

**Security Layers:**
1. **HTTPS/TLS** - Transport encryption
2. **CORS** - Cross-origin request validation
3. **Input Validation** - Sanitize incoming data
4. **Rate Limiting** - Prevent DoS attacks
5. **RLS (Row Level Security)** - Database-level access control
6. **API Keys** - Separate anon/service role keys
7. **Authentication** - Optional Supabase Auth

---

## Monitoring & Logging

```
Components Monitored:
├── Vercel
│   ├─ Function invocations
│   ├─ Response times
│   ├─ Error rates
│   └─ Bandwidth usage
│
├── Supabase
│   ├─ Database queries
│   ├─ Connection count
│   ├─ Storage usage
│   └─ API calls
│
└── Frontend
    ├─ Page load time
    ├─ Real-time sync latency
    ├─ Browser errors (console)
    └─ User interactions
```

---

## Scaling Strategy

```
Current Setup (Single Node):
│
├─ Single ESP32 → Single Vercel → Single Database
│
└─ Supports: 1-5 sensor nodes

Multi-Sensor Setup:
│
├─ Multiple ESP32s → Vercel Function → Supabase
│
└─ Supports: 10-50 sensor nodes (No additional cost)

Enterprise Setup:
│
├─ Sensor mesh network
├─ Multiple gateways
├─ Load balancer
├─ Database replication
│
└─ Supports: 100+ sensor nodes (Requires upgrade)
```

---

## Performance Metrics

```
Baseline Performance:
├─ Frontend load time: 1-2 seconds
├─ Real-time latency: <500ms
├─ API response time: <200ms
├─ Database query time: <100ms
├─ CDN cache hit ratio: 95%+
└─ Uptime SLA: 99.9%

With Vercel & Supabase:
├─ Auto-scaling ✓
├─ Global CDN ✓
├─ Automatic backups ✓
├─ Load balancing ✓
└─ Real-time features ✓
```

---

## Future Enhancements

```
Phase 1 (Current):
├─ Real-time monitoring
├─ Historical data
└─ Alert system

Phase 2 (Next):
├─ Mobile app (React Native)
├─ Advanced analytics
├─ Machine learning predictions
├─ Custom dashboard builder
└─ User authentication

Phase 3 (Long-term):
├─ Multi-location dashboard
├─ API marketplace
├─ Integration with IoT platforms
├─ Blockchain data integrity
└─ Decentralized storage (IPFS)
```

---

*Last Updated: June 2026*
*Architecture Version: 2.0*
