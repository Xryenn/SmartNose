/**
 * Vercel Serverless Function - Sensor Data Endpoint
 * Endpoint: POST /api/readings
 * 
 * Menerima data dari ESP32 sensor dan menyimpan ke Supabase
 */

export default async function handler(req, res) {
  // Only allow POST requests
  if (req.method !== 'POST') {
    return res.status(405).json({ error: 'Method not allowed' });
  }

  try {
    const { createClient } = await import('@supabase/supabase-js');
    
    // Get environment variables
    const SUPABASE_URL = process.env.SUPABASE_URL;
    const SUPABASE_SERVICE_ROLE_KEY = process.env.SUPABASE_SERVICE_ROLE_KEY || process.env.SUPABASE_ANON_KEY;

    if (!SUPABASE_URL || !SUPABASE_SERVICE_ROLE_KEY) {
      return res.status(500).json({ error: 'Missing Supabase credentials' });
    }

    // Initialize Supabase client
    const supabase = createClient(SUPABASE_URL, SUPABASE_SERVICE_ROLE_KEY);

    // Parse request body
    const {
      device_id,
      ch4_ppm,
      h2s_ppm,
      temperature,
      humidity,
      latitude,
      longitude,
      gps_valid
    } = req.body;

    // Validate required fields
    if (!device_id || ch4_ppm === undefined || h2s_ppm === undefined || 
        temperature === undefined || humidity === undefined) {
      return res.status(400).json({ error: 'Missing required fields' });
    }

    // Insert data ke Supabase
    const { data, error } = await supabase
      .from('sensor_readings')
      .insert([
        {
          device_id,
          ch4_ppm: parseFloat(ch4_ppm),
          h2s_ppm: parseFloat(h2s_ppm),
          temperature: parseFloat(temperature),
          humidity: parseFloat(humidity),
          latitude: latitude ? parseFloat(latitude) : null,
          longitude: longitude ? parseFloat(longitude) : null,
          gps_valid: gps_valid || false,
          alert_ch4: parseFloat(ch4_ppm) > 1500, // Adjust threshold as needed
          alert_h2s: parseFloat(h2s_ppm) > 50     // Adjust threshold as needed
        }
      ])
      .select();

    if (error) {
      console.error('Supabase error:', error);
      return res.status(500).json({ error: 'Failed to insert data', details: error });
    }

    // Check untuk alerts
    const thresholds = {
      ch4_red: 1500,
      h2s_red: 50
    };

    if (parseFloat(ch4_ppm) > thresholds.ch4_red || parseFloat(h2s_ppm) > thresholds.h2s_red) {
      // Log alert
      await supabase
        .from('alerts_log')
        .insert([
          {
            device_id,
            alert_type: parseFloat(ch4_ppm) > thresholds.ch4_red ? 'CH4' : 'H2S',
            severity: 'CRITICAL',
            ppm_value: parseFloat(ch4_ppm) > thresholds.ch4_red ? ch4_ppm : h2s_ppm
          }
        ]);
    }

    return res.status(200).json({ 
      success: true, 
      message: 'Data recorded successfully',
      id: data[0].id 
    });

  } catch (error) {
    console.error('Error:', error);
    return res.status(500).json({ error: 'Internal server error', message: error.message });
  }
}
